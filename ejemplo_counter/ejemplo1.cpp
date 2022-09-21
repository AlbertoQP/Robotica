#include "ejemplo1.h"

ejemplo1::ejemplo1(): Ui_Counter()
{
	setupUi(this);
	show();
	connect(button, SIGNAL(clicked()), this, SLOT(doButton()) );
    connect(button_2, SIGNAL(clicked()), this, SLOT(doReset()) );
    connect(timer, SIGNAL(timeout()), this, SLOT(contador()));
}

void ejemplo1::doButton()
{
	qDebug() << "click on Start/Stop button";

    if (timer -> isActive() == false) {
        timer -> start(1000);
    }
    else{
        timer -> stop();
    }
}

void ejemplo1::doReset()
{
    qDebug() << "click on Reset button";
    time = 0;
    lcdNumber->display(time);
}

void ejemplo1::contador()
{
    time++;
    lcdNumber->display(time);
}




