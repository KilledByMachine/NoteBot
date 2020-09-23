#include "note_bot.h"


using namespace Telegram;


Note_bot::Note_bot(QString token,  qlonglong interval, QObject *parent) :
    TarnaBot(token,  interval, parent)
{
    qDebug()<< connect(this, &Note_bot::updateReceived, this, &Note_bot::handleUpdate);
    qDebug() << sizeof(Update);
    nextOp=noOp;
    db = QSqlDatabase::addDatabase("QSQLITE");


        db.setDatabaseName("E:/test");

        if (!db.open())
        {
            qDebug() << db.lastError().text();
        }
        QSqlQuery query;

        //query.exec("select * from  table_name"); !?!?!?
        while (query.next())
        {
                             int id = query.value(0).toInt();
                             int name = query.value(1).toInt();
                             QString lastname = query.value(2).toString();

                             qDebug() << id << name<<lastname<<endl;
        }

}

void Note_bot::handleUpdate(Update update)
{

  qDebug()<<" handleUpd; path:"<<QDir::current();
    if(update.hasMessage() && update.getMessage().hasText())
    {
        QString messageText=update.getMessage().getText();
        qint64 chatId=update.getMessage().getChat().getId();
        sendMessage(chatId,messageText);
        qDebug()<<"Mess is: "+messageText;
        switch (nextOp) {
            case new_:
                newNote(update);
                break;
            case read_:
              readNote(update);
              break;
            case del_:
                 delNote(update);
                 break;
            case edit_:
                 editNote(update);
                 break;
            case noOp:
            break;

        }

        if(update.getMessage().getText().startsWith('/'))
        {
            qDebug()<<"Starts with /";
            decode(update);

        }

    }

}

void Note_bot::decode(Update upd)
{
    qint64 chatId=upd.getMessage().getChat().getId();
    if(upd.getMessage().getText().startsWith("/new")){
        if(upd.getMessage().getText().size() >4){
            newNote(upd);
            return;
        }
        sendMessage(chatId,"Put the name of new note:");

        nextOp = new_;
        return;
    }
    if(upd.getMessage().getText().startsWith("/read")){
      if(upd.getMessage().getText().size() >5){
        readNote(upd);
        return;
      }
      sendMessage(chatId,"What you want to read?:");
      nextOp = read_;
      return;
    }
    if(upd.getMessage().getText().startsWith("/del")){

        if(upd.getMessage().getText().size() >4){
            delNote(upd);
            return;
        }
        sendMessage(chatId,"Put the name of del note:");
        nextOp = del_;
        return;

    }
    if(upd.getMessage().getText().startsWith("/edit")){
        if(upd.getMessage().getText().size() >5){
            qDebug()<<upd.getMessage().getText();       //передати перший парам з повідомленням
            QString name = upd.getMessage().getText().split(" ")[1];
            editNote(upd, name);
            nextOp = edit_;
            return;
        }
        sendMessage(chatId,"Put the name of edit note:");
        nextOp = edit_;

        return;
    }
    if(upd.getMessage().getText().startsWith("/list")){

        //send all notes (noteName) for that user. if numbers >10, create table for scroling
        list(upd);
        return;
    }


    sendMessage(chatId,"Unknown command!");

}


void Note_bot::newNote(Update update) {
  nextOp = noOp;

  QDir userFolder;
  userFolder.setPath("./data");


  qDebug() << "comand: newNote";

  qint64 chatId = update.getMessage().getChat().getId();
  if (!userFolder.exists(QString::number(chatId))) {
    userFolder.mkdir(QString::number(chatId));
  }

  QString oldPath = QDir::currentPath();
  QDir::setCurrent(userFolder.path() + "/" + QString::number(chatId));

  QFile noteFile;
  if (update.getMessage().getText().startsWith('/')) {

    noteFile.setFileName(update.getMessage().getText().split(" ")[1]);
  }
  else {
    noteFile.setFileName(update.getMessage().getText());
  }

  noteFile.open(QIODevice::WriteOnly | QIODevice::Text);
  sendMessage(chatId, "Created!" );
  noteFile.close();
  qDebug() << " NEW; path:" << QDir::current();
  /*

  QTextStream out(&res);
  out << text << "\n";
  res.close();
  */
  QDir::setCurrent(oldPath);
  return;
}

void Note_bot::readNote(Update update)
{
  nextOp = noOp;

  QDir userFolder;
  userFolder.setPath("./data");

  qDebug() << "comand: readNote";

  qint64 chatId = update.getMessage().getChat().getId();
  if (!userFolder.exists(QString::number(chatId))) {
    sendMessage(chatId, "No notes for this user" );

    return;
  }

  QString oldPath = QDir::currentPath();
  QDir::setCurrent(userFolder.path() + "/" + QString::number(chatId));

  QFile readfile;
  if (update.getMessage().getText().startsWith('/')) {

    readfile.setFileName(update.getMessage().getText().split(" ")[1]);
  }
  else {
    readfile.setFileName(update.getMessage().getText());
  }

  readfile.open(QIODevice::ReadOnly | QIODevice::Text);
  QString reply;
  reply = readfile.readAll();
  readfile.close();
  sendMessage(chatId, reply);
  QDir::setCurrent(oldPath);
  return;

}

void Note_bot::delNote(Update update)
{
  QDir userFolder;
  userFolder.setPath("./data");
  QString delfilename;
  if (update.getMessage().getText().startsWith('/')) {
    delfilename = update.getMessage().getText().split(" ")[1];
  }
  else
    delfilename = update.getMessage().getText();

  QString oldPath = QDir::currentPath();
  qint64 chatId = update.getMessage().getChat().getId();
  QDir::setCurrent(userFolder.path() + "/" + QString::number(chatId));
  if (QFile::exists(delfilename)) {
    QFile::remove(delfilename);
    sendMessage(chatId, "Deleted!" );
  }
  else {
    qDebug() << "File to del not exist!!!";
    sendMessage(chatId, "Error: note doesn't exist !" );
  }

  qDebug()<<"DEL_DEL_DEL";
  nextOp = noOp;
  QDir::setCurrent(oldPath);
}

void Note_bot::editNote(Update update, QString text)
{
    static int count=0;
    static QString noteName("");
    static QString noteText("");

    qint64 chatId = update.getMessage().getChat().getId();
    switch (count) {
    case 0: {
      if (text != "") {
        noteName = text;
      }
      else {
        noteName = update.getMessage().getText();
      }
      QString oldPath = QDir::currentPath();
      QDir::setCurrent(QString("./data") + "/" + QString::number(chatId));
      QFile readfile(noteName); // change to @read by nub,er of note@
      readfile.open(QIODevice::ReadOnly | QFile::Text);
      QString reply;
      reply = readfile.readAll();
      sendMessage(chatId, "Old text =" + reply);
      sendMessage(chatId, "Put new text:");
      QDir::setCurrent(oldPath);
      }
            break;
        case 1:
        noteText =  update.getMessage().getText();
            break;
    }
    count++;

    qDebug()<<count;
    qDebug()<<noteName<<" "<<noteText;

    if(noteText.size() && noteName.size()){

         nextOp = noOp;
         count = 0;
         QString oldPath = QDir::currentPath();
         QDir::setCurrent(QString("./data") + "/" + QString::number(chatId));
         QFile writefile;
         writefile.setFileName(noteName);
         writefile.open(QIODevice::WriteOnly | QFile::Text);
         QByteArray a;
         writefile.write(noteText.toLocal8Bit()); // convert
         writefile.close();
         sendMessage(chatId, "Saved!");
         QDir::setCurrent(oldPath);
    }

}

void Note_bot::list(Update update)
{
  qint64 chatId = update.getMessage().getChat().getId();

  QDir userFolder(QString("./data") + "/" + QString::number(chatId));

  QStringList notelist = userFolder.entryList(QStringList(), QDir::Files);
  QString reply;
  foreach (QString filename, notelist) {
    qDebug() << filename;
    reply += filename+'\n';
  }

  sendMessage(chatId, reply);

}

Note_bot::~Note_bot()
{
    db.close();

}
