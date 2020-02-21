
// TO DO - IF MIDI PORT NAME DOES NOT MATCH, SEARCH FOR CLOSEST MATCH
// RESEPOND TO PROGRAM MENU SIGNAL PC 0 CH 16
// FIND HOW TO CLOSE THE MIDI PORT




/*!
 * \file MainWindow.h
 * \author masc4ii
 * \copyright 2018
 * \brief The main window
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qmidiin.h"                                                             // ADDED BY rockumk
#include "qmidiout.h"
#include "qmidimapper.h"
#include <QTableWidget>
#include <QRecentFilesMenu.h>
#include "EventReturnFilter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void onMidiMessageReceive(QMidiMessage* message);

private slots:
    void on_actionSearchInterfaces_triggered();
    void on_actionAboutSysexLive_triggered();
    void on_actionAboutQt_triggered();
    void on_actionAddEntry_triggered();
    void on_actionDeleteEntry_triggered();
    void on_tableWidget_doubleClicked(const QModelIndex &index);                    // ADDED BY rockumk
    // void on_tableWidget_Clicked(const QModelIndex &index);                       // ADDED BY rockumk BUT MY SINGLE CLICK METHOD FAILED
    void on_actionSendPatches_triggered();
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_comboBoxSynth1_activated(const QString &arg1);
    void on_comboBoxSynth2_activated(const QString &arg1);
    void on_comboBoxSynth3_activated(const QString &arg1);                          // ADDED BY rockumk
    void on_comboBoxSynth4_activated(const QString &arg1);                          // ADDED BY rockumk
    void on_comboBoxMidiIn_activated(const QString &arg1);                          // ADDED BY rockumk
    void on_actionMoveUp_triggered();
    void on_actionMoveDown_triggered();
    void loadFile(const QString &fileName);
    void on_plainTextEdit_textChanged();
    //  void on_plainTextEdit2_textChanged();                    // ADDING DID NOT SEEM NEEDED ADDED BUT ALSO REMOVED BY rockumk
    void on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void on_actionZoomTextPlus_triggered();
    void on_actionZoomTextMinus_triggered();

private:
    Ui::MainWindow *ui;
    void getPorts(void);
    void searchSynths(void);
    void moveRow( bool up );
    void readSettings(void);
    void writeSettings(void);
    QList<QTableWidgetItem*>  takeRow( int row );
    void setRow(int row, const QList<QTableWidgetItem *> &rowItems);

    QRecentFilesMenu *m_recentFilesMenu;
    QString m_lastSaveFileName;
    QString m_synth1;
    QString m_synth2;
    QString m_synth3;                                                               // ADDED BY rockumk
    QString m_synth4;                                                               // ADDED BY rockumk
    QString m_midiinput;                                                            // ADDED BY rockumk
    QMidiIn *m_midiIn;                                                              // ADDED BY rockumk
    QMidiOut *m_midiOut;                                                            // ADDED BY rockumk
    EventReturnFilter *m_eventFilter;
};

#endif // MAINWINDOW_H
