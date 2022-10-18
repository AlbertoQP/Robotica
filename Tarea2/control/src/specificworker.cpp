/*
 *    Copyright (C) 2022 by YOUR NAME HERE
 *
 *    This file is part of RoboComp
 *
 *    RoboComp is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.s
 *
 *    RoboComp is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "specificworker.h"

const int UMBRAL_DISTANCIA = 1000;
bool flag = false;

/**
* \brief Default constructor
*/
SpecificWorker::SpecificWorker(TuplePrx tprx, bool startup_check) : GenericWorker(tprx)
{
    this->startup_check_flag = startup_check;
    // Uncomment if there's too many debug messages
    // but it removes the possibility to see the messages
    // shown in the console with qDebug()
//	QLoggingCategory::setFilterRules("*.debug=false\n");
}

/**
* \brief Default destructor
*/
SpecificWorker::~SpecificWorker()
{
    std::cout << "Destroying SpecificWorker" << std::endl;
}

bool SpecificWorker::setParams(RoboCompCommonBehavior::ParameterList params)
{
//	THE FOLLOWING IS JUST AN EXAMPLE
//	To use innerModelPath parameter you should uncomment specificmonitor.cpp readConfig method content
//	try
//	{
//		RoboCompCommonBehavior::Parameter par = params.at("InnerModelPath");
//		std::string innermodel_path = par.value;
//		innerModel = std::make_shared(innermodel_path);
//	}
//	catch(const std::exception &e) { qFatal("Error reading config params"); }

    return true;
}

void SpecificWorker::initialize(int period)
{
    std::cout << "Initialize worker" << std::endl;
    this->Period = period;
    if(this->startup_check_flag)
    {
        this->startup_check();
    }
    else
    {
        timer.start(Period);
    }

}
/*
int check_maxdistance(int a, int b, int c)
{
    // a > b > c -> 0
    // b > a > c -> 1
    // c > a > b -> 2
    if (a > b && a > c)
        return 0;
    else if (b > a && b > c)
        return 1;
    else if (c > a && c > b)
        return 2;
}*/


std::tuple<SpecificWorker::TipoModo, float, float> SpecificWorker::modo_esquivar(RoboCompLaserMulti::TLaserData &ldata)
{
    // El robot piensa que va a hacer
    // Ordenar por distancia la seccion central del laser1
    RoboCompLaserMulti::TLaserData central(ldata.begin() + ldata.size()/3, (ldata.end() - ldata.size()/3)-1);
    RoboCompLaserMulti::TLaserData der(ldata.end() - ldata.size()/3, ldata.end());
    RoboCompLaserMulti::TLaserData izq(ldata.begin(), (ldata.begin() + ldata.size()/3)-1);
    std::ranges::sort(central, {}, &RoboCompLaserMulti::TData::dist);
    std::ranges::sort(der, {}, &RoboCompLaserMulti::TData::dist);
    std::ranges::sort(izq, {}, &RoboCompLaserMulti::TData::dist);

    float adv = 0.0;
    float rot = 0.0;
    std::cout<<"Distancia: "<<central.front().dist<<std::endl;

    /*
    if((central.front().dist < UMBRAL_DISTANCIA))
    {
        adv = 0.0;
        rot = 0.5;
    }
    else
    {
        adv = 600.0;
        rot = 0.0;
    }*/


    if((central.front().dist < UMBRAL_DISTANCIA))
    {
        adv = 0.0;
        rot = 0.5;

        if(der.front().dist < izq.front().dist)
        {
            adv = 0.0;
            rot = -0.5;
        }
    }
    else
    {
        adv = 600.0;
        rot = 0.0;
    }

    /*
    if((central.front().dist > UMBRAL_DISTANCIA))
    {
        if (check_maxdistance(central.front().dist, der.front().dist, izq.front().dist) == 1) { adv = 0.0; rot = 0.5; }
        if (check_maxdistance(central.front().dist, der.front().dist, izq.front().dist) == 2) { adv = 0.0; rot = -0.5; }
    }

    adv = 600.0;
    rot = 0.0;
    */

    auto tuple = std::make_tuple(TipoModo::Esquivar, adv, rot);
    return tuple;
}

std::tuple<SpecificWorker::TipoModo, float, float> SpecificWorker::modo_paredes(RoboCompLaserMulti::TLaserData &ldata)
{

}

std::tuple<SpecificWorker::TipoModo, float, float> SpecificWorker::modo_espiral(RoboCompLaserMulti::TLaserData &ldata)
{

}

void SpecificWorker::compute()
{
    // El robot siente
    RoboCompLaserMulti::TLaserData ldata;
    try
    {
        ldata = lasermulti_proxy->getLaserData(3);
    }
    catch (const Ice::Exception & e) { std::cout<<e.what() << std::endl; return;};

    // Modo - Adv - Rot
    std::tuple<TipoModo, float, float> result;
    TipoModo modo;
    modo = TipoModo::Esquivar;

    switch (modo)
    {
        case TipoModo::Idle:
            break;
        case TipoModo::Esquivar:
            std::cout << "Modo esquivar" << std::endl;
            result = modo_esquivar(ldata);
            break;
        case TipoModo::Paredes:
            std::cout << "Modo paredes" << std::endl;
            modo_paredes(ldata);
            break;
        case TipoModo::Espiral:
            std::cout << "Modo espiral" << std::endl;
            modo_espiral(ldata);
            break;
    }
    modo = std::get<TipoModo>(result);

    // El robot actua
    try
    {
        differentialrobotmulti_proxy->setSpeedBase(3, std::get<1>(result), std::get<2>(result));
    }
    catch (const Ice::Exception & e) { std::cout<<e.what() << std::endl; return;}
}

int SpecificWorker::startup_check()
{
    std::cout << "Startup check" << std::endl;
    QTimer::singleShot(200, qApp, SLOT(quit()));
    return 0;
}




/**************************************/
// From the RoboCompDifferentialRobot you can call this methods:
// this->differentialrobot_proxy->correctOdometer(...)
// this->differentialrobot_proxy->getBasePose(...)
// this->differentialrobot_proxy->getBaseState(...)
// this->differentialrobot_proxy->resetOdometer(...)
// this->differentialrobot_proxy->setOdometer(...)
// this->differentialrobot_proxy->setOdometerPose(...)
// this->differentialrobot_proxy->setSpeedBase(...)
// this->differentialrobot_proxy->stopBase(...)

/**************************************/
// From the RoboCompDifferentialRobot you can use this types:
// RoboCompDifferentialRobot::TMechParams

/**************************************/
// From the RoboCompLaser you can call this methods:
// this->laser_proxy->getLaserAndBStateData(...)
// this->laser_proxy->getLaserConfData(...)
// this->laser_proxy->getLaserData(...)

/**************************************/
// From the RoboCompLaser you can use this types:
// RoboCompLaser::LaserConfData
// RoboCompLaser::TData