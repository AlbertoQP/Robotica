#ifndef ejemplo1_H
#define ejemplo1_H

#include <QtGui>
#include "ui_counterDlg.h"

class ejemplo1 : public QWidget, public Ui_Counter
{
    Q_OBJECT
    public:
        ejemplo1();

    public slots:
        void doButton();
        void contador();
        void cambiarTiempo(int);
        void doReset();
        void doAddtime();
        void doCuentaAtras();

    private:
        QTimer *timer = new QTimer(this);
        int time = 0;
        int periodo = 1000;
        int time2 = 0;
};

#endif // ejemplo1_H
