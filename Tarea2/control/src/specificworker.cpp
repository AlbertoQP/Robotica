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

const int UMBRAL_COLISION = 950;
const float MAX_SPEED = 900.0;

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

void SpecificWorker::compute()
{
    // El robot siente
    RoboCompLaserMulti::TLaserData ldata;
    try
    { ldata = lasermulti_proxy->getLaserData(3); }
    catch (const Ice::Exception & e) { std::cout<<e.what() << std::endl; return;};

    TipoModo modo = std::get<0>(result);

    switch (modo)
    {
        default:
            std:: cout << "Modo idle" << std::endl;
            result = modo_idle(ldata);
            break;
        case TipoModo::Avanzar:
            std::cout << "Modo avanzar" << std::endl;
            result = modo_avanzar(ldata);
            break;
        case TipoModo::Girar:
            std::cout << "Modo girar" << std::endl;
            result = modo_girar(ldata);
            break;
            /*case TipoModo::Paredes:
                std::cout << "Modo paredes" << std::endl;
                modo_paredes(ldata);
                break;
            case TipoModo::Espiral:
                std::cout << "Modo espiral" << std::endl;
                modo_espiral(ldata);
                break;*/
    }
    modo = std::get<TipoModo>(result);

    // El robot actua
    auto &[_, adv, rot] = result;
    try
    { differentialrobotmulti_proxy->setSpeedBase(3, adv, rot); }
    catch (const Ice::Exception & e) { std::cout<<e.what() << std::endl; return;}
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
SpecificWorker::Action SpecificWorker::modo_idle(const RoboCompLaserMulti::TLaserData &ldata)
{
    auto tuple = std::make_tuple(TipoModo::Avanzar, MAX_SPEED, 0);
    return tuple;
}

SpecificWorker::Action SpecificWorker::modo_avanzar(const RoboCompLaserMulti::TLaserData &ldata)
{
    RoboCompLaserMulti::TLaserData central(ldata.begin() + ldata.size()/3, (ldata.end() - ldata.size()/3)-1);
    RoboCompLaserMulti::TLaserData der(ldata.end() - ldata.size()/3, ldata.end() - 150);
    RoboCompLaserMulti::TLaserData izq(ldata.begin() + 150, (ldata.begin() + ldata.size()/3)-1);
    std::ranges::sort(central, {}, &RoboCompLaserMulti::TData::dist);
    std::ranges::sort(der, {}, &RoboCompLaserMulti::TData::dist);
    std::ranges::sort(izq, {}, &RoboCompLaserMulti::TData::dist);

    float adv = 0.0;
    float rot = 0.0;
    std::cout<<"Distancia: "<<central.front().dist<<std::endl;

    TipoModo modo = TipoModo::Avanzar;

    if((central.front().dist < UMBRAL_COLISION))
    {
        modo = TipoModo::Girar;
        adv = 100.0;

        if (der.front().dist > izq.front().dist)
            rot = -1.0;
        else
            rot = 1.0;

        return std::make_tuple(modo, adv, rot);
    }

    //REVISAR HACER EL MODO ESPIRAL
/*
    if (auto sum = std::accumulate(ldata.begin(), ldata.end(), 0.f, [](auto s, auto a)
        {return s += a.dist;}); sum > ldata.size() * 4000 * 0.8)
    {
        modo = TipoModo::Espiral;
        adv = 50.0;
        rot = 0.25;

        return std::make_tuple(modo, adv, rot);
    }*/
    modo = TipoModo::Avanzar;

    adv = MAX_SPEED;
    rot = 0.0;

    return std::make_tuple(modo, adv, rot);
}


SpecificWorker::Action SpecificWorker::modo_girar(const RoboCompLaserMulti::TLaserData &ldata)
{
    RoboCompLaserMulti::TLaserData central(ldata.begin() + ldata.size()/3, (ldata.end() - ldata.size()/3)-1);
    std::ranges::sort(central, {}, &RoboCompLaserMulti::TData::dist);

    std::cout<<"Distancia: "<<central.front().dist<<std::endl;

    TipoModo modo = TipoModo::Girar;

    if((central.front().dist > UMBRAL_COLISION))
    {
        // AVANZAR O PAREDES
        modo = TipoModo::Avanzar;

        float adv = MAX_SPEED;
        float rot = 0.0;

        return std::make_tuple(modo, adv, rot);
    }
    else
    {
        return result;
    }
}

SpecificWorker::Action SpecificWorker::modo_paredes(const RoboCompLaserMulti::TLaserData &ldata)
{
    RoboCompLaserMulti::TLaserData central(ldata.begin() + ldata.size()/3, (ldata.end() - ldata.size()/3)-1);
    RoboCompLaserMulti::TLaserData der(ldata.end() - ldata.size()/3, ldata.end() - 150);
    RoboCompLaserMulti::TLaserData izq(ldata.begin() + 150, (ldata.begin() + ldata.size()/3)-1);
    std::ranges::sort(central, {}, &RoboCompLaserMulti::TData::dist);
    std::ranges::sort(der, {}, &RoboCompLaserMulti::TData::dist);
    std::ranges::sort(izq, {}, &RoboCompLaserMulti::TData::dist);

    float adv = 0.0;
    float rot = 0.0;
    TipoModo modo = TipoModo::Paredes;

    if((central.front().dist < UMBRAL_COLISION))
    {
        modo = TipoModo::Girar;
        adv = 100.0;

        if (der.front().dist > izq.front().dist)
            rot = -1.0;
        else
            rot = 1.0;

        return std::make_tuple(modo, adv, rot);
    }

    // IR GIRANDO IZQUIERDA DERECHA CONTINUAMENTE PARA SIMULAR IR RECTO

}

SpecificWorker::Action SpecificWorker::modo_espiral(const RoboCompLaserMulti::TLaserData &ldata)
{

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