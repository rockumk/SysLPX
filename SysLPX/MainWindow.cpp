/*!
 * \file MainWindow.cpp
 * \author masc4ii
 * \copyright 2018
 * \brief The main window
 */

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QBuffer>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QXmlStreamWriter>
#include "DarkStyle.h"

#define APPNAME "SysexLive"
#define VERSION "0.1"

//Constructor
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //We dont want a context menu which could disable the menu bar
    setContextMenuPolicy(Qt::NoContextMenu);

    //Apply DarkStyle
#if QT_VERSION >= 0x050000
    CDarkStyle::assign();
#endif

    m_midiOut = new QMidiOut( this );
    m_midiIn = new QMidiIn( this );                                       // ADDED BY rockumk

    getPorts();

    m_lastSaveFileName = QDir::homePath();

    ui->plainTextEdit->setEnabled( false );

    //AutoResize for table columns
#if QT_VERSION >= 0x050000
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#else
    ui->tableWidget->horizontalHeader()->setResizeMode( QHeaderView::Stretch );
#endif
    ui->tableWidget->hideColumn( 1 );
    ui->tableWidget->hideColumn( 2 );
    ui->tableWidget->hideColumn( 3 );                                       // ADDED BY rockumk
    ui->tableWidget->hideColumn( 4 );                                       // ADDED BY rockumk

    //RecentFilesMenu
    m_recentFilesMenu = new QRecentFilesMenu( tr( "Recent Files" ), ui->menuFile );
    readSettings();
    connect( m_recentFilesMenu, SIGNAL(recentFileTriggered(const QString &)), this, SLOT(loadFile(const QString &)) );
    ui->menuFile->insertMenu( ui->actionSave, m_recentFilesMenu );

    //Keyfilter on Table
    m_eventFilter = new EventReturnFilter( this );
    ui->tableWidget->installEventFilter( m_eventFilter );


    QString path = "C:\\Users\\Rock\\Documents\\default.syxml";                                          // ADDED BY rockumk
    loadFile(path);

    connect(m_midiIn, SIGNAL(midiMessageReceived(QMidiMessage*)), this, SLOT(onMidiMessageReceive(QMidiMessage*)));   // ADDED BY rockumk
}

//Destructor
MainWindow::~MainWindow()
{
    writeSettings();
    delete m_eventFilter;
    delete m_midiOut;
    delete m_midiIn;                                                                    // ADDED BY rockumk
    delete ui;
}

void MainWindow::onMidiMessageReceive(QMidiMessage *message)                            // ADDED BY rockumk
{
    int statusType;
    int midiChannel;
    int programNumber;
    int theRowCount;
    statusType = (message->getStatus());
    midiChannel = (message->getChannel());
    programNumber = (message->getPitch());
    theRowCount = ui->tableWidget->rowCount();

    if (statusType == 192 && midiChannel == 16)
    {
     if ( programNumber < theRowCount)
        {
    ui->plainTextEdit2->appendPlainText("Received Program Change on MIDI Channel " +QString::number(midiChannel)+ " - "+QString::number(programNumber));
    ui->tableWidget->selectRow(programNumber);
    on_actionSendPatches_triggered();
    QTableWidgetItem *item = ui->tableWidget->item( programNumber, 10);
    item->setText("");
        }
     else
     {
       ui->plainTextEdit2->appendPlainText("Received Program Change on MIDI Channel " +QString::number(midiChannel)+ " - "+QString::number(programNumber));
       ui->plainTextEdit2->appendPlainText("However, there isn't a program number "+QString::number(programNumber) + " to select...");
     }
    }


}

//Get Ports and write into combobox
void MainWindow::getPorts( void )
{
    bool portAvailable = true;
    ui->comboBoxSynth1->addItems( m_midiOut->getPorts() );
    ui->comboBoxSynth2->addItems( m_midiOut->getPorts() );
    ui->comboBoxSynth3->addItems( m_midiOut->getPorts() );                    // ADDED BY rockumk
    ui->comboBoxSynth4->addItems( m_midiOut->getPorts() );                    // ADDED BY rockumk
    ui->comboBoxMidiIn->addItems( m_midiIn->getPorts() );                     // ADDED BY rockumk

    //Block GUI if no port available
    if( ui->comboBoxSynth1->count() == 0 )
    {
        portAvailable = false;
        statusBar()->showMessage( tr( "No MIDI port found." ), 0 );                 //??  SHOUD A CHECK ALSO BE DONE FOR INPUTS?
    }
    else
    {
        statusBar()->showMessage( tr( "" ), 0 );
    }
    ui->comboBoxSynth1->setEnabled( portAvailable );
    ui->comboBoxSynth2->setEnabled( portAvailable );
    ui->comboBoxSynth3->setEnabled( portAvailable );                    // ADDED BY rockumk
    ui->comboBoxSynth4->setEnabled( portAvailable );                    // ADDED BY rockumk
    ui->comboBoxMidiIn->setEnabled( portAvailable );                    // ADDED BY rockumk
    ui->labelSynth1->setEnabled( portAvailable );
    ui->labelSynth2->setEnabled( portAvailable );
    ui->labelSynth3->setEnabled( portAvailable );                    // ADDED BY rockumk
    ui->labelSynth4->setEnabled( portAvailable );                    // ADDED BY rockumk
    //ui->labelMidiIn->setEnabled( portAvailable );                    // ?? NEEDED? I WAS UNABLE TO ADD rockumk
    ui->actionSendPatches->setEnabled( portAvailable );
}

//Connect ports which were saved in file
void MainWindow::searchSynths( void )
{
    for( int i = 0; i < ui->comboBoxSynth1->count(); i++ )
    {
        if( ui->comboBoxSynth1->itemText( i ) == m_synth1 )
        {
            ui->comboBoxSynth1->setCurrentIndex( i );
            break;
        }
    }

    for( int i = 0; i < ui->comboBoxSynth2->count(); i++ )
    {
        if( ui->comboBoxSynth2->itemText( i ) == m_synth2 )
        {
            ui->comboBoxSynth2->setCurrentIndex( i );
            break;
        }
    }

    for( int i = 0; i < ui->comboBoxSynth3->count(); i++ )                    // ADDED BY rockumk
    {
        if( ui->comboBoxSynth3->itemText( i ) == m_synth3 )
        {
            ui->comboBoxSynth3->setCurrentIndex( i );
            break;
        }
    }

    for( int i = 0; i < ui->comboBoxSynth4->count(); i++ )                    // ADDED BY rockumk
    {
        if( ui->comboBoxSynth4->itemText( i ) == m_synth4 )
        {
            ui->comboBoxSynth4->setCurrentIndex( i );
            break;
        }
    }

    for( int i = 0; i < ui->comboBoxMidiIn->count(); i++ )                    // ADDED BY rockumk
    {
        if( ui->comboBoxMidiIn->itemText( i ) == m_midiinput )
        {
            ui->comboBoxMidiIn->setCurrentIndex( i );
            break;
        }
    }




}

//Find the ports
void MainWindow::on_actionSearchInterfaces_triggered()
{
    ui->comboBoxSynth1->clear();
    ui->comboBoxSynth2->clear();
    ui->comboBoxSynth3->clear();                    // ADDED BY rockumk
    ui->comboBoxSynth4->clear();                    // ADDED BY rockumk
    ui->comboBoxMidiIn->clear();                    // ADDED BY rockumk

    getPorts();
    searchSynths();
}

//About Box
void MainWindow::on_actionAboutSysexLive_triggered()
{
    QMessageBox::about( this, QString( "About %1" ).arg( APPNAME ),
                                QString(
                                  "<html>"
                                  "<body><h3>%1</h3>"
                                  " <p>%1 v%2</p>"
                                  " <p>%3</p>"
                                  " <p>See <a href='%4'>this site</a> for more information.</p>"
                                  " <p>Darkstyle Copyright (c) 2017, <a href='%5'>Juergen Skrotzky</a></p>"
                                  " <p>Icons by <a href='%6'>Double-J Design</a> under <a href='%7'>CC4.0</a></p>"
                                  " </body></html>" )
                                 .arg( APPNAME )
                                 .arg( VERSION )
                                 .arg( "by masc." )
                                 .arg( "https://github.com/masc4ii/SysexLive" )
                                 .arg( "https://github.com/Jorgen-VikingGod" )
                                 .arg( "http://www.doublejdesign.co.uk/" )
                                 .arg( "https://creativecommons.org/licenses/by/4.0/" ) );
}

//Qt Box
void MainWindow::on_actionAboutQt_triggered()
{
    QMessageBox::aboutQt(this);
}

//Add entry to table
void MainWindow::on_actionAddEntry_triggered()
{
    ui->tableWidget->insertRow( ui->tableWidget->rowCount() );
    for( int i = 0; i < ui->tableWidget->columnCount(); i++ )
    {
        QTableWidgetItem *item = new QTableWidgetItem( "" );
        if( i > 0 && i < 5 ) item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem( ui->tableWidget->rowCount()-1, i, item );
    }
    ui->plainTextEdit->setEnabled( true );
    ui->plainTextEdit2->setEnabled( true );                                             // ADDED BY rockumk
}

//Delete current entry in table
void MainWindow::on_actionDeleteEntry_triggered()
{
    ui->tableWidget->removeRow( ui->tableWidget->currentRow() );
}

//Doubleclick in table
void MainWindow::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    if( index.column() == 5 || index.column() == 6 || index.column() == 7 || index.column() == 8 )                      // ALTERED BY rockumk FROM 2 3 to 5 6 7 8
    {
        QString path = QFileInfo( m_lastSaveFileName ).absolutePath();
        if( !QDir( path ).exists() ) path = QDir::homePath();

        QString fileName = QFileDialog::getOpenFileName(this,
                                               tr("Open sysex file"), path,
                                               tr("Sysex files (*.syx)"));

        //Abort selected
        if( fileName.count() == 0 ) return;

        m_lastSaveFileName = fileName;

        QTableWidgetItem *item = ui->tableWidget->item( index.row(), index.column() - 4 );                      // ALTERED BY rockumk FROM - 2 to -4
        item->setText( fileName );
        item = ui->tableWidget->item( index.row(), index.column() );
        item->setText( QFileInfo( fileName ).fileName() );
    }

    else if(index.column() == 10)                                                                               // ADDED BY rockumk (SINGLE CLICK METHOD FAILED)
    {
        on_actionSendPatches_triggered();
        QTableWidgetItem *item = ui->tableWidget->item( index.row(), index.column());
        item->setText("");
    }
}

/*
//Singleclick in table                                                                                          // ADDED BY rockumk - DID NOT WORK
void MainWindow::on_tableWidget_Clicked(const QModelIndex &index)
{
    if(index.column() == 10)
    {
        on_actionSendPatches_triggered();
        QTableWidgetItem *item = ui->tableWidget->item( index.row(), index.column());
    }
}
*/


//Send current selected
void MainWindow::on_actionSendPatches_triggered()
{
    //Nothing there? End!
    if( ui->tableWidget->rowCount() == 0 ) return;

    //Get current row
    int row = ui->tableWidget->currentRow();

    //No row selected
    if( row < 0 )
    {
        QMessageBox::information( this, APPNAME, tr( "Please select an entry first!" ) );
        return;
    }

    for( int i = 0; i < 4; i++ )                    // ALTERED BY rockumk  FROM     "for( int i = 0; i < 2; i++ )"
    {
        //Get filename
        QString fileName = ui->tableWidget->item( row, i + 1 )->text();

        //Get & check file
        QFile file( fileName );
        if( !file.exists() ) continue;

        //Read from file
        file.open( QIODevice::ReadOnly );
        QByteArray syxData = file.readAll();
        file.close();

        //Send to synth 1
        if( i == 0 && ui->comboBoxSynth1->count() != 0 )
        {
            m_midiOut->openPort( ui->comboBoxSynth1->currentIndex() );
            std::vector<unsigned char> message( syxData.begin(), syxData.end() );
            m_midiOut->sendRawMessage( message );
            m_midiOut->closePort();
        }
        //Send to synth 2
        if( i == 1 && ui->comboBoxSynth2->count() != 0 )
        {
            m_midiOut->openPort( ui->comboBoxSynth2->currentIndex() );
            std::vector<unsigned char> message( syxData.begin(), syxData.end() );
            m_midiOut->sendRawMessage( message );
            m_midiOut->closePort();
        }
        //Send to synth 3
        if( i == 2 && ui->comboBoxSynth3->count() != 0 )                    // ADDED BY rockumk
        {
            m_midiOut->openPort( ui->comboBoxSynth3->currentIndex() );
            std::vector<unsigned char> message( syxData.begin(), syxData.end() );
            m_midiOut->sendRawMessage( message );
            m_midiOut->closePort();
        }
        //Send to synth 4
        if( i == 3 && ui->comboBoxSynth4->count() != 0 )                    // ADDED BY rockumk
        {
            m_midiOut->openPort( ui->comboBoxSynth4->currentIndex() );
            std::vector<unsigned char> message( syxData.begin(), syxData.end() );
            m_midiOut->sendRawMessage( message );
            m_midiOut->closePort();
        }
    }
}

//Delete table
void MainWindow::on_actionNew_triggered()
{
    while( ui->tableWidget->rowCount() > 0 )
    {
        ui->tableWidget->removeRow( 0 );
    }
    ui->plainTextEdit->setEnabled( false );
    ui->plainTextEdit2->setEnabled( false );                        // ADDED BY rockumk
}

//Open table
void MainWindow::on_actionOpen_triggered()
{
    QString path = QFileInfo( m_lastSaveFileName ).absolutePath();
    QString fileName = QFileDialog::getOpenFileName(this,
                                           tr("Open syxml"), path,
                                           tr("sysex table xml (*.syxml)"));

    //Abort selected
    if( fileName.count() == 0 ) return;

    m_recentFilesMenu->addRecentFile( fileName );
    m_lastSaveFileName = fileName;

    loadFile( fileName );
}

//Load file
void MainWindow::loadFile(const QString & fileName)
{
    //Open file
    QFile file(fileName);
    if( !file.exists() )
    {
        m_recentFilesMenu->removeFile( fileName );
        return;
    }

    //Clear table
    on_actionNew_triggered();

    QXmlStreamReader Rxml;
    file.open(QIODevice::ReadOnly | QFile::Text);

    Rxml.setDevice(&file);
    while( !Rxml.atEnd() && !Rxml.isEndDocument() )
    {
        Rxml.readNext();
        //qDebug()<<"In while!";

        if( Rxml.isStartElement() && Rxml.name() == "settings" )
        {
            //Read name string, if there is one
            if( Rxml.attributes().count() >= 5 )                                           //??
            {
                m_synth1 = Rxml.attributes().at(0).value().toString();
                m_synth2 = Rxml.attributes().at(1).value().toString();
                m_synth3 = Rxml.attributes().at(2).value().toString();                    // ADDED BY rockumk
                m_synth4 = Rxml.attributes().at(3).value().toString();                    // ADDED BY rockumk
                m_midiinput = Rxml.attributes().at(4).value().toString();                 // ADDED BY rockumk
                //qDebug() << "Ports:" << m_synth1 << m_synth2 << m_synth3 << m_synth4 << m_midiinput;                    // ALTERED BY rockumk
                searchSynths();
            }

            //bool checkInOpen;
            //checkInOpen = m_midiIn->isPortOpen();
            //if (checkInOpen)
            //    {
            //   m_midiIn->closePort();
            //    }
            //                                         TRIED BUT FAILED TO CHECK AND CLOSE PORT
           if(m_midiIn->isPortOpen()) m_midiIn->closePort();
            m_midiIn->openPort(m_midiinput);                                                        // ?? DO PORTS GET CLOSED WHEN CHANGED??        rockumk
            m_midiIn->setIgnoreTypes(1, 1, 1);                                                      // IS THIS THE BEST TIME TO OPEN THE PORT?      rockumk

            while( !Rxml.atEnd() && !Rxml.isEndElement() )
            {
                Rxml.readNext();
                if( Rxml.isStartElement() && Rxml.name() == "song" )
                {
                    //qDebug()<<"start!"<<Rxml.name();
                    on_actionAddEntry_triggered();

                    //Read name string, if there is one
                    if( Rxml.attributes().count() != 0 )
                    {
                        ui->tableWidget->item( ui->tableWidget->rowCount()-1, 0 )->setText( Rxml.attributes().at(0).value().toString() );
                    }

                    while( !Rxml.atEnd() && !Rxml.isEndElement() )
                    {
                        Rxml.readNext();
                        if( Rxml.isStartElement() && Rxml.name() == "synth1" )                    // COL INDEX ALTERED BY rockumk
                        {
                            QString fileName = Rxml.readElementText();
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 1 )->setText( fileName );
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 5 )->setText( QFileInfo( fileName ).fileName() );
                            Rxml.readNext();
                        }
                        else if( Rxml.isStartElement() && Rxml.name() == "synth2" )                    // COL INDEX ALTERED BY rockumk
                        {
                            QString fileName = Rxml.readElementText();
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 2 )->setText( fileName );
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 6 )->setText( QFileInfo( fileName ).fileName() );
                            Rxml.readNext();
                        }
                        else if( Rxml.isStartElement() && Rxml.name() == "synth3" )                    // ADDED BY rockumk
                        {
                            QString fileName = Rxml.readElementText();
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 3 )->setText( fileName );
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 7 )->setText( QFileInfo( fileName ).fileName() );
                            Rxml.readNext();
                        }
                        else if( Rxml.isStartElement() && Rxml.name() == "synth4" )                    // ADDED BY rockumk
                        {
                            QString fileName = Rxml.readElementText();
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 4 )->setText( fileName );
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 8 )->setText( QFileInfo( fileName ).fileName() );
                            Rxml.readNext();
                        }
                        else if( Rxml.isStartElement() && Rxml.name() == "info" )                    // COL INDEX ALTERED BY rockumk
                        {
                            QString text = Rxml.readElementText();
                            ui->tableWidget->item( ui->tableWidget->rowCount()-1, 9 )->setText( text );
                            Rxml.readNext();
                        }
                        else if( Rxml.isStartElement() ) //future features
                        {
                            Rxml.readElementText();
                            Rxml.readNext();
                        }
                    }
                    Rxml.readNext();
                }
                else if( Rxml.isEndElement() )
                {
                    //qDebug() << "EndElement! (song)";
                    Rxml.readNext();
                }
            }
        }
    }

    file.close();
}

//Save table
void MainWindow::on_actionSave_triggered()
{
    QString path = QFileInfo( m_lastSaveFileName ).absolutePath();
    QString fileName = QFileDialog::getSaveFileName(this,
                                           tr("Save syxml"), path,
                                           tr("sysex table xml (*.syxml)"));

    //Abort selected
    if( fileName.count() == 0 ) return;

    m_recentFilesMenu->addRecentFile( fileName );
    m_lastSaveFileName = fileName;

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    //Open a XML writer
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement( "settings" );
    xmlWriter.writeAttribute( "port1", m_synth1 );
    xmlWriter.writeAttribute( "port2", m_synth2 );
    xmlWriter.writeAttribute( "port3", m_synth3 );                                                      // ADDED BY rockumk
    xmlWriter.writeAttribute( "port4", m_synth4 );                                                      // ADDED BY rockumk
    xmlWriter.writeAttribute( "InputPort", m_midiinput );                                               // ADDED BY rockumk

    for( int i = 0; i < ui->tableWidget->rowCount(); i++ )
    {
        xmlWriter.writeStartElement( "song" );
        xmlWriter.writeAttribute( "name", ui->tableWidget->item(i, 0)->text() );
        xmlWriter.writeTextElement( "synth1", ui->tableWidget->item(i, 1)->text() );
        xmlWriter.writeTextElement( "synth2", ui->tableWidget->item(i, 2)->text() );
        xmlWriter.writeTextElement( "synth3", ui->tableWidget->item(i, 3)->text() );                    // ADDED BY rockumk
        xmlWriter.writeTextElement( "synth4", ui->tableWidget->item(i, 4)->text() );                    // ADDED BY rockumk
        xmlWriter.writeTextElement( "info", ui->tableWidget->item(i, 9)->text() );                      // ALTERED BY rockumk INDEX CHANGED FROM 4 to 9
        xmlWriter.writeEndElement();
    }
    xmlWriter.writeEndElement();

    xmlWriter.writeEndDocument();

    file.close();
}

//Actively changed port 1
void MainWindow::on_comboBoxSynth1_activated(const QString &arg1)
{
    m_synth1 = arg1;
}

//Actively changed port 2
void MainWindow::on_comboBoxSynth2_activated(const QString &arg1)
{
    m_synth2 = arg1;
}

//Actively changed port 3
void MainWindow::on_comboBoxSynth3_activated(const QString &arg1)                             // ADDED BY rockumk
{
    m_synth3 = arg1;
}

//Actively changed port 4
void MainWindow::on_comboBoxSynth4_activated(const QString &arg1)                             // ADDED BY rockumk
{
    m_synth4 = arg1;
}

//Actively changed Input Port
void MainWindow::on_comboBoxMidiIn_activated(const QString &arg1)                             // ADDED BY rockumk
{
    m_midiinput = arg1;

    //bool checkInOpenAgain;
    //checkInOpenAgain = m_midiIn->isPortOpen();
    //if (checkInOpenAgain)
    //    {
    //   m_midiIn->closePort();
    //    }                                                     TRIED BUT FAILED TO CHECK AND CLOSE PORT
    if(m_midiIn->isPortOpen()) m_midiIn->closePort();
    m_midiIn->openPort(m_midiinput);                            // ?? DO PORTS GET CLOSED WHEN CHANGED?? rockumk
    m_midiIn->setIgnoreTypes(1, 1, 1);                                                   // IS THIS THE BEST TIME TO OPEN THE PORT?
}

//Move row up
void MainWindow::on_actionMoveUp_triggered()
{
    moveRow( true );
}

//Move row down
void MainWindow::on_actionMoveDown_triggered()
{
    moveRow( false );
}

//Move row
void MainWindow::moveRow(bool up)
{
    if(ui->tableWidget->selectedItems().count() != 1) return;
    const int sourceRow = ui->tableWidget->row(ui->tableWidget->selectedItems().at(0));
    const int destRow = (up ? sourceRow-1 : sourceRow+1);
    if(destRow < 0 || destRow >= ui->tableWidget->rowCount()) return;

    // take whole rows
    QList<QTableWidgetItem*> sourceItems = takeRow(sourceRow);
    QList<QTableWidgetItem*> destItems = takeRow(destRow);

    // set back in reverse order
    setRow(sourceRow, destItems);
    setRow(destRow, sourceItems);

    ui->tableWidget->setCurrentCell( destRow, ui->tableWidget->currentColumn() );
}

//Read registry settings
void MainWindow::readSettings()
{
    QSettings set( QSettings::UserScope, "masc.SysexLive", "SysexLive" );
    restoreGeometry( set.value( "mainWindowGeometry" ).toByteArray() );
    ui->splitter->restoreState( set.value( "splitterState" ).toByteArray() );
    m_lastSaveFileName = set.value( "lastFileName", QDir::homePath() ).toString();
    m_recentFilesMenu->restoreState( set.value( "recentFiles" ).toByteArray() );
    QFont font = ui->plainTextEdit->font();
    font.setPointSize( set.value( "fontSize", font.pointSize() ).toInt() );
    ui->plainTextEdit->setFont( font );
}

//Write registry settings
void MainWindow::writeSettings()
{
    QSettings set( QSettings::UserScope, "masc.SysexLive", "SysexLive" );
    set.setValue( "mainWindowGeometry", saveGeometry() );
    set.setValue( "splitterState", ui->splitter->saveState() );
    set.setValue( "lastFileName", m_lastSaveFileName );
    set.setValue( "recentFiles", m_recentFilesMenu->saveState() );
    set.setValue( "fontSize", ui->plainTextEdit->font().pointSize() );
}

//Take row (for move)
QList<QTableWidgetItem*>  MainWindow::takeRow(int row)
{
    QList<QTableWidgetItem*> rowItems;
    for (int col = 0; col < ui->tableWidget->columnCount(); ++col)
    {
        rowItems << ui->tableWidget->takeItem(row, col);
    }
    return rowItems;
}

//Set row (for move)
void MainWindow::setRow(int row, const QList<QTableWidgetItem*>& rowItems)
{
    for (int col = 0; col < ui->tableWidget->columnCount(); ++col)
    {
        ui->tableWidget->setItem(row, col, rowItems.at(col));
    }
}

//Write accords from Edit to Table
void MainWindow::on_plainTextEdit_textChanged()
{
    if( ui->tableWidget->currentRow() >= 0 )
    {
        ui->tableWidget->item( ui->tableWidget->currentRow(), 5 )->setText( ui->plainTextEdit->toPlainText() );
    }
    else
    {
        ui->plainTextEdit->blockSignals( true );
        ui->plainTextEdit->setPlainText( "" );
        ui->plainTextEdit->blockSignals( false );
    }
}

//Current cell changed
void MainWindow::on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED( currentColumn );
    Q_UNUSED( previousRow );
    Q_UNUSED( previousColumn );
    if( currentRow < 0 )
    {
        ui->plainTextEdit->setPlainText( QString( "" ) );
        ui->plainTextEdit->setEnabled( false );
    }
    else
    {
        ui->plainTextEdit->blockSignals( true );
        ui->plainTextEdit->setPlainText( ui->tableWidget->item( currentRow, 5 )->text() );
        ui->plainTextEdit->blockSignals( false );
        ui->plainTextEdit->setEnabled( true );
    }
}

//Zoom Text +
void MainWindow::on_actionZoomTextPlus_triggered()
{
    QFont font = ui->plainTextEdit->font();
    font.setPointSize( font.pointSize() + 1 );
    ui->plainTextEdit->setFont( font );
}

//Zoom Text -
void MainWindow::on_actionZoomTextMinus_triggered()
{
    QFont font = ui->plainTextEdit->font();
    font.setPointSize( font.pointSize() - 1 );
    ui->plainTextEdit->setFont( font );
}
