#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QCheckBox>
#include <QMainWindow>
#include <QPlainTextEdit>

#include "qmidiin.h"
#include "qmidiout.h"
#include "messagecomposer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    QMidiIn *_midiIn;
    QMidiOut *_midiOut;

    QPlainTextEdit *_inConsole;
    QComboBox *_inPortComboBox;

    QComboBox *_outPortComboBox;
    MessageComposer *_messageComposer;

    QCheckBox *_forwardCheckBox;

public slots:
    void onMidiMessageReceive(QMidiMessage* message);
private slots:
    void onInOpenPortButtonClicked(bool value);
    void onInOpenVirtualPortButtonClicked(bool value);
    void onOutOpenPortButtonClicked(bool value);
    void onOutOpenVirtualPortButtonClicked(bool value);
    void onSendMessageButtonClicked(bool value);                                // ADDED BY ROCKUM

};

#endif // MAINWINDOW_H
