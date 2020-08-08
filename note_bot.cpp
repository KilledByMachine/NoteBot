#include "note_bot.h"


using namespace Telegram;


Note_bot::Note_bot(QString token,  qlonglong interval, QObject *parent) :
    TarnaBot(token,  interval, parent)
{
    qDebug()<< connect(this, &Note_bot::updateReceived, this, &Note_bot::handleUpdate);
    nextOp=noOp;
    db = QSqlDatabase::addDatabase("QSQLITE");
        /*
        QDir a;
        qDebug()<<a.path();
        a.mkdir("first");
        if(!a.exists("data"))
        {
            a.setPath(a.path()+"/data");

        }
        qDebug()<<a.path();
        a.mkdir("sec");
        a.setPath(".");
        a.mkdir("three");
        //a.rmdir("gmg");

    */
        db.setDatabaseName("E:/test");

        if (!db.open())
        {
            qDebug() << db.lastError().text();
        }
        QSqlQuery query;

        query.exec("select * from  table_name");
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

  qDebug()<<" handleUpd; path:"<<QDir::current();;
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
            //qDebug()<<update.getMessage().getText().size();
            decode(update);

        }

    }

}

void Note_bot::decode(Update upd)
{
    qint64 chatId=upd.getMessage().getChat().getId();
    // можливо змінити параметри декоду, на кожну команду мб окремий метод, для створення зробити нормальну поведінку
    // для нових записів, тобтостворення папки. папкипід юзера якщо нема, збереження
    // в видаленні мб з видаленням всеї папки.. но не факт, з едітом.. видавати весь текст, і дозволяти його зміну/або
    // повне видаленя і перезапис вмісту, мб зробити потом конфіги для юзерів. аля завжди заміняти/доп в текст

    if(upd.getMessage().getText().startsWith("/new")){
        /*
         * insert into db
         * create file in folder @idUser@ named by biggest num in notes table
        */
        if(upd.getMessage().getText().size() >4){
            newNote(upd);
            return;
        }
        sendMessage(chatId,"Put the name of new note:");

        nextOp = new_;
        return;
    }
    if(upd.getMessage().getText().startsWith("/del")){

        //find note by noteName, if exist - del, else send "problem"
        if(upd.getMessage().getText().size() >4){
            delNote(upd);
            return;
        }
        sendMessage(chatId,"Put the name of del note:");
        nextOp = del_;
        return;

    }
    if(upd.getMessage().getText().startsWith("/edit")){

        //find note by noteName, send text from file, wait for new filling
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

  // qDebug() << "Us path" << userFolder.path() << oldPath;

  QDir::setCurrent(userFolder.path() + "/" + QString::number(chatId));

  // qDebug() << "After set curr"  << QDir::currentPath();

  // find in db first
  if (QFile::exists(delfilename)) {
    QFile::remove(delfilename);
  }
  else {
    qDebug() << "File to del not exist!!!";
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

    switch (count){
        case 0:
        if(text!=""){
            noteName = text;
        }
        else
        {
            noteName =  update.getMessage().getText();
        }
        sendMessage(update.getMessage().getChat().getId(),"Put new text:");
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
         count  = 0;

    }
    //db works


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

Note_bot:: ~Note_bot(){
    db.close();

}
