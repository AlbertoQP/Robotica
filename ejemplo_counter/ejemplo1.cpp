#include "ejemplo1.h"

ejemplo1::ejemplo1(): Ui_Counter()
{
	setupUi(this);
	show();
	connect(button, SIGNAL(clicked()), this, SLOT(doButton()) );
    connect(button_2, SIGNAL(clicked()), this, SLOT(doReset()));

    connect(horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(cambiarTiempo(int)));

    connect(button_4, SIGNAL(clicked()), this, SLOT(doAddtime()));
    connect(button_5, SIGNAL(clicked()), this, SLOT(doAddtime()));
    connect(button_6, SIGNAL(clicked()), this, SLOT(doCuentaAtras()));

    connect(timer, SIGNAL(timeout()), this, SLOT(contador()));
}

void ejemplo1::doButton()
{
	qDebug() << "click on Start/Stop button";

    if (timer -> isActive() == false) {
        timer -> start(periodo);
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

void ejemplo1::cambiarTiempo(int a)
{
    periodo = a*100;
    qDebug() << periodo;
}

void ejemplo1::doAddtime()
{
    qDebug() << "click on AddTime button";
    time2 += 10;
    lcdNumber_2->display(time2);

}

void ejemplo1::contador()
{
    time++;
    lcdNumber -> display(time);
}

void ejemplo1::doCuentaAtras()
{
   time2--;  
   lcdNumber_2 -> display(time2);

}




