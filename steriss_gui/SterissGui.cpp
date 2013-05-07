/*
* Copyright (c) 2012 Marius Zwicker
* All rights reserved.
* 
* @LICENSE_HEADER_START:GPL@
* 
* This file is part of Steriss.
* 
* Steriss is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* Steriss is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*  
* You should have received a copy of the GNU General Public License
* along with Steriss. If not, see <http://www.gnu.org/licenses/>.
* 
* @LICENSE_HEADER_END:GPL@
*/

#include "SterissGui.h"
#include "ui_SterissGui.h"

#include <QMenu>
#include <QAction>
#include <QCoreApplication>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDebug>
#include <QProcess>
#include <QMessageBox>
#include <QColorDialog>

#include <stdint.h>

SterissGui::SterissGui(QWidget *parent)
  : QWidget(parent), ui(new Ui::SterissGui), _steriss(new QProcess(this)), _verbose(false) {
  ui->setupUi(this);

  // signals
  connect( ui->cmdLoadConfig, SIGNAL(clicked()), this, SLOT(loadConfig()) );
  connect( ui->cmdSaveConfig, SIGNAL(clicked()), this, SLOT(saveConfig()) );
  connect( ui->cmdRunConfig, SIGNAL(clicked()), this, SLOT(runConfig()) );
  connect( ui->cmdPickInput, SIGNAL(clicked()), this, SLOT(pickInput()) );
  connect( ui->cmdPickOutput, SIGNAL(clicked()), this, SLOT(pickOutput()) );
  connect( ui->cmdPickBinary, SIGNAL(clicked()), this, SLOT(pickBinary()) );
  connect( ui->cmdColorOverlay, SIGNAL(clicked()), this, SLOT(pickColorOverlay()) );

  // default values
  _current_config["input"] = "";
  _current_config["output"] = "";
  _current_config["threshold"] = "117.6";
  _current_config["border_threshold"] = "235.3";
  _current_config["algorithm"] = "plain";
  _current_config["voxelfile"] = false;
  _current_config["overlay_color"] = "000000000";
  _current_config["output_stem"] = "pgm";
#if _WIN32
# if DEBUG
  _binary = qApp->applicationDirPath() + "\\sterissD.exe";
# else
  _binary = qApp->applicationDirPath() + "\\steriss.exe";
# endif
#else
# if DEBUG
  _binary = qApp->applicationDirPath() + "/sterissD";
# else
  _binary = qApp->applicationDirPath() + "/steriss";
# endif
#endif
  _verbose = false;

  updateGuiFromConfig ();
}

SterissGui::~SterissGui() {
  delete ui;
}

void SterissGui::saveConfig() {

  updateConfigFromGui();

  QString fileName = QFileDialog::getSaveFileName(this,
                                                  tr("Save the current config"),
                                                  "steriss.conf",
                                                  tr("Config File (*.conf)"));

  if( fileName.isEmpty() )
    return;

  QFile file( fileName );
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    return;

  QTextStream out(&file);
  foreach( QString key, _current_config.keys() ) {
    QVariant& value = _current_config[key];

    if( value.type() == QVariant::Bool )
      out << "--" << (value.toBool() ? key : "no" + key) << "\n";
    else
      out << "--" << key << "=" << value.toString() << "\n";
  }

}

void SterissGui::loadConfig() {
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Load an existing config"),
                                                  "steriss.conf",
                                                  tr("Config File (*.conf)"));

  if( fileName.isEmpty() )
    return;

  QFile file( fileName );
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return;

  QTextStream in(&file);
  while (!in.atEnd()) {
    QString line = in.readLine();

    if( line.isEmpty() )
      continue;

    QStringList line_lst = line.split("=");
    QString key_raw = line_lst.first();
    QString key_name = line_lst.first().remove("--no").remove("--");
    QString key_value = line_lst.last();
    if(! _current_config.contains (key_name)) {
      QMessageBox::critical(this,tr("Invalid Config"),tr("The config file is not valid, unknown parameter: ") + key_name);
      return;
    }

    QVariant& config_value = _current_config[key_name];

    if( config_value.type() == QVariant::Bool )
      config_value = (key_raw.contains("--no") == false);
    else
      config_value = key_value;

    //qDebug() << "reading" << key << "as" << value;
  }

  updateGuiFromConfig();
}

void SterissGui::runConfig() {
  updateConfigFromGui();
  ui->txtProgramLog->clear();
  _errors.clear();


  delete _steriss;
  _steriss = new QProcess(this);
  connect( _steriss, SIGNAL(readyReadStandardOutput()), this, SLOT(newProcessOutput()) );
  connect( _steriss, SIGNAL(readyReadStandardError()), this, SLOT(newProcessError()) );
  connect( _steriss, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)) );
  connect( _steriss, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)) );

  QString run_command = "Running " + _binary;

  QStringList arguments;
  foreach( QString key, _current_config.keys() ) {
    QVariant& value = _current_config[key];

    if( value.type() == QVariant::Bool ) {
      run_command += " --" + (value.toBool() ? key : "no" + key);
      arguments << "--" + (value.toBool() ? key : "no" + key) << "\n";
    } else {
      run_command += " -" + key + " " + value.toString();
      arguments << "-" + key << value.toString();
    }

  }
  if( _verbose ) {
    run_command += " -verbose";
    arguments << "-verbose";
  }

  ui->txtProgramLog->appendPlainText( run_command + "\n\n===================================================================================\n");
  if(! validateConfig())
    return;


  _steriss->start( _binary, arguments );

  ui->layoutBinary->setEnabled(false);
  ui->layoutActions->setEnabled(false);
  ui->tabConfiguration->setEnabled( false );
  ui->chkVerbose->setEnabled(false);
  ui->tabWidget->setCurrentIndex(1);
}

bool SterissGui::validateConfig() {
  if(   _current_config["input"].toString().isEmpty() ||
        _current_config["output"].toString().isEmpty() ||
        _current_config["threshold"].toString().isEmpty() ||
        _current_config["border_threshold"].toString().isEmpty() ||
        _current_config["algorithm"].toString().isEmpty() ) {
    QMessageBox::critical(this, tr("Invalid Config"), tr("Please make sure to set all options"));
    return false;
  } else if ( ! QFile::exists (_binary) ) {
    QMessageBox::critical (this, tr("Invalid Binary"), tr("The steriss binary select does not exist"));
    return false;
  } else
    return true;
}

void SterissGui::processError (QProcess::ProcessError error){
  switch( error ) {
    case QProcess::FailedToStart:
      {
        newProcessError ();
        QMessageBox::critical(this, tr("Error running Sterris"), tr("<b>Steriss failed to start:</b><br/>Could not execute given binary"));
      }
      break;
    case QProcess::Crashed:
      {
        newProcessError ();
        QMessageBox::critical(this, tr("Error running Sterris"), tr("<b>Steriss crashed</b>"));
      }
      break;
    default:
      {
        QMessageBox::information(this, tr("Finished"), tr("Steriss finished running") );
      }
  }

  ui->layoutBinary->setEnabled(true);
  ui->layoutActions->setEnabled(true);
  ui->tabConfiguration->setEnabled( true );
  ui->chkVerbose->setEnabled(true);
}

void SterissGui::processFinished(int exitCode, QProcess::ExitStatus exitStatus) {

  switch( exitStatus ) {
    case QProcess::CrashExit:
      {
        newProcessError ();
        QMessageBox::critical(this, tr("Error running Sterris"), tr("<b>Sterris terminated too early:</b><br/>") + _errors);
      }
      break;
    default:
      {
        if( exitCode != 0 )
          QMessageBox::information(this, tr("Ended"), tr("Steriss experienced some error, please see the log") );
        else
          QMessageBox::information(this, tr("Finished"), tr("Steriss finished running") );
      }
  }

  ui->layoutBinary->setEnabled(true);
  ui->layoutActions->setEnabled(true);
  ui->tabConfiguration->setEnabled( true );
  ui->chkVerbose->setEnabled( true );
}

void SterissGui::updateConfigFromGui() {

  _current_config["input"] = ui->txtInput->text();
  _current_config["output"] = ui->txtOutput->text();
  _current_config["threshold"] = ui->txtThreshold->text();
  _current_config["border_threshold"] = ui->txtBorderThreshold->text();
  _current_config["algorithm"] = ui->cboAlgorithm->currentText();
  _current_config["voxelfile"] = ui->chk3dObj->isChecked();
  _current_config["output_stem"] = ui->cboOutputFormat->currentText();
  _binary = ui->txtBinary->text();
  _verbose = ui->chkVerbose->isChecked ();
}


inline int cboIndexOf(QComboBox* cbo, const QString& val) {
  for(int i = 0; i < cbo->count(); ++i)
    if( cbo->itemText(i) == val )
      return i;

  return -1;
}

void SterissGui::updateGuiFromConfig() {
  ui->txtInput->setText( _current_config.value ("input").toString() );
  ui->txtOutput->setText( _current_config.value ("output").toString() );
  ui->txtThreshold->setText(  _current_config.value ("threshold").toString()  );
  ui->txtBorderThreshold->setText( _current_config.value ("border_threshold").toString() );
  ui->cboAlgorithm->setCurrentIndex( cboIndexOf( ui->cboAlgorithm, _current_config.value ("algorithm").toString() ) );
  ui->cboOutputFormat->setCurrentIndex( cboIndexOf( ui->cboOutputFormat, _current_config.value ("output_stem").toString() ) );
  ui->txtBinary->setText( _binary );
  ui->chk3dObj->setChecked( _current_config.value("voxelfile").toBool() );
  ui->chkVerbose->setChecked ( _verbose );

  int64_t current_value = _current_config.value("overlay_color").toLongLong();
  QColor current_color;
  current_color.setRed( current_value / 1000000 % 1000 );
  current_color.setGreen( current_value / 1000 % 1000 );
  current_color.setBlue( current_value % 1000 );

  QString stylesheet = QString("color: rgb(%1, %2, %3);").arg(current_color.red()).arg(current_color.green()).arg(current_color.blue());

  ui->cmdColorOverlay->setStyleSheet( stylesheet );
  ui->cmdColorOverlay->setText( QString("%1").arg(current_value));
}

void SterissGui::pickBinary() {
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Select Steriss Executable"),
                                                  ui->txtBinary->text(),
                                                  tr("Steriss (steriss.exe, steriss, sterissD, sterissD.exe)"));

  if( !fileName.isEmpty() )
    ui->txtBinary->setText( fileName );
}

void SterissGui::pickInput() {
  QString directory = QFileDialog::getExistingDirectory(this,
                                                        tr("Select Input Folder"),
                                                        ui->txtInput->text(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if( !directory.isEmpty() )
    ui->txtInput->setText( directory );
}

void SterissGui::pickOutput() {
  QString directory = QFileDialog::getExistingDirectory(this,
                                                        tr("Select Output Folder"),
                                                        ui->txtOutput->text(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  if( !directory.isEmpty() )
    ui->txtOutput->setText( directory );
}

void SterissGui::pickColorOverlay() {

  updateConfigFromGui();

  int64_t current_value = _current_config.value("overlay_color").toLongLong();
  QColor current_color;
  current_color.setRed( current_value / 1000000 % 1000 );
  current_color.setGreen( current_value / 1000 % 1000 );
  current_color.setBlue( current_value % 1000 );

  QColor new_color = QColorDialog::getColor( current_color, this, tr("Select Color Overlay") );
  int64_t new_value = new_color.red() * 1000000 + new_color.green() * 1000 + new_color.blue();

  _current_config["overlay_color"] = QString("%1").arg(new_value);
  if( new_color != QColor(0,0,0) )
    _current_config["output_stem"] = "png";
  updateGuiFromConfig();
}

void SterissGui::newProcessOutput() {
  QString new_message = _steriss->readAllStandardOutput ();
  new_message.chop (1);
  ui->txtProgramLog->appendPlainText( new_message );
}

void SterissGui::newProcessError() {
  QString new_error = _steriss->readAllStandardError();
  new_error.chop (1);
  ui->txtProgramLog->appendHtml ("<p style=\"color:red;\">" + new_error + "</p>" );
  _errors += new_error;
}
