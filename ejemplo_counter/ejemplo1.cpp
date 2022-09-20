#include "ejemplo1.h"

ejemplo1::ejemplo1(): Ui_Counter()
{
	setupUi(this);
	show();
	connect(button, SIGNAL(clicked()), this, SLOT(doButton()) );

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(contador()));
    timer->start(1000);
}

void ejemplo1::doButton()
{
	qDebug() << "click on button";
}

void ejemplo1::contador()
{
    time++;
    lcdNumber->display(time);
}




