#ifndef NOTE_BOT_H
#define NOTE_BOT_H
#include <TarnaBot>
#include <QNetworkProxy>
#include <QtSql>

#include <iostream>


namespace Telegram
{

    class Note_bot :public TarnaBot
    {
    public:
        enum NextOp {
            new_,
            read_,
            del_,
            edit_,
            noOp
        };
        Note_bot(QString token, QNetworkProxy proxy, qlonglong interval, QObject* parent = nullptr);

        Note_bot(QString token, qlonglong interval, QObject* parent = nullptr);
        ~Note_bot();
    public slots:
        void handleUpdate(Update update);      
    private:
        QSqlDatabase db;
        void decode(Update upd);
        NextOp nextOp;
        void newNote   (Update);
        void readNote   (Update);
        void delNote   (Update);
        void editNote  (Update, QString ="");
        void list      (Update);

    };

}


#endif // NOTE_BOT_H
