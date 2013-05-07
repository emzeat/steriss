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

#ifndef STERISSGUI_H
#define STERISSGUI_H

#include <QWidget>
#include <QHash>
#include <QString>
#include <QProcess>
#include <QVariant>

namespace Ui {
class SterissGui;
}

class SterissGui : public QWidget
{
    Q_OBJECT
    
  public:
    explicit SterissGui(QWidget *parent = 0);
    ~SterissGui();

    bool validateConfig();

  public slots:
    void saveConfig();
    void loadConfig();
    void runConfig();

    void pickBinary();
    void pickInput();
    void pickOutput();
    void pickColorOverlay();

    void updateConfigFromGui();
    void updateGuiFromConfig();

    void newProcessOutput();
    void newProcessError();
    void processError(QProcess::ProcessError);
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    
  private:
    Ui::SterissGui *ui;

    QHash<QString,QVariant> _current_config;
    QString _binary;
    QProcess* _steriss;
    QString _errors;
    bool _verbose;
};

#endif // STERISSGUI_H
