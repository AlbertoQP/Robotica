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

const int UMBRAL_COLISION = 1050;
const int VELOCIDAD_ESPIRAL = 400;
const int UMBRAL_MURO = 1000;
const int DELTA = 50;
const float MAX_SPEED = 1200.0;

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
    std::cout << "COMPUTE - MODO: " << static_cast<std::underlying_type<TipoModo>::type>(modo) << std::endl;

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
        case TipoModo::Paredes:
            std::cout << "Modo paredes" << std::endl;
            result = modo_paredes(ldata);
            break;
        case TipoModo::Espiral:
            std::cout << "Modo espiral" << std::endl;
            result = modo_espiral(ldata);
            break;
    }

    // El robot actua
    auto &[_, adv, rot] = result;
    try
    { differentialrobotmulti_proxy->setSpeedBase(3, adv, rot); }
    catch (const Ice::Exception & e) { std::cout<<e.what() << std::endl; return;}
}

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

    // Modo Espiral
    if (auto sum = std::accumulate(ldata.begin(), ldata.end(), 0.f, [](auto s, auto a)
        {return s += a.dist;}); sum > ldata.size() * 4000 * 0.8)
    {
        modo = TipoModo::Espiral;
        adv = 100.0;
        rot = 0.5;

        return std::make_tuple(modo, adv, rot);
    }
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
        int choice;
        std::srand(time(0));
        choice = rand() % 2;

        switch(choice)
        {
            case 0:
                modo = TipoModo::Avanzar;
                break;
            case 1:
                modo = TipoModo::Paredes;
                break;
            default:
                modo = TipoModo::Avanzar;
                break;
        }

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

    std::cout<<"Distancia: "<<central.front().dist<<std::endl;

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
    //Nos alejamos del muro
    if((der.front().dist < UMBRAL_MURO - DELTA))
    {
        modo = TipoModo::Paredes;
        adv = MAX_SPEED;
        rot = -0.2;
        return std::make_tuple(modo, adv, rot);
    }

    //Nos acercamos al muro
    else if(der.front().dist >= UMBRAL_MURO + DELTA)
    {
        modo = TipoModo::Paredes;
        adv = MAX_SPEED;
        rot = 0.2;
        return std::make_tuple(modo, adv, rot);
    }
    //No hacemos nada, ya va paralelo al muro. Devolvemos la tupla
    else
    {
        return result;
    }
}

SpecificWorker::Action SpecificWorker::modo_espiral(const RoboCompLaserMulti::TLaserData &ldata)
{
    RoboCompLaserMulti::TLaserData central(ldata.begin() + ldata.size()/3, (ldata.end() - ldata.size()/3)-1);
    RoboCompLaserMulti::TLaserData der(ldata.end() - ldata.size()/3, ldata.end() - 150);
    RoboCompLaserMulti::TLaserData izq(ldata.begin() + 150, (ldata.begin() + ldata.size()/3)-1);
    std::ranges::sort(central, {}, &RoboCompLaserMulti::TData::dist);
    std::ranges::sort(der, {}, &RoboCompLaserMulti::TData::dist);
    std::ranges::sort(izq, {}, &RoboCompLaserMulti::TData::dist);

    std::cout<<"Distancia: "<<central.front().dist<<std::endl;

    float adv = 0.0;
    float rot = 1.0;
    TipoModo modo = TipoModo::Espiral;

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
    else
    {
        // TODO: Controlar velocidad de avance y giro máximo
        adv = std::get<1>(result);
        rot = std::get<2>(result);
        if(adv < VELOCIDAD_ESPIRAL){
            adv += 1.2;
        }

        if(rot > 0.0){
            rot -= 0.0005;
        }
        std::get<1>(result) = adv;
        std::get<2>(result) = rot;

        std::cout << "adv: " <<  adv << " rot: " << rot << std::endl;
        return std::make_tuple(modo, adv, rot);
    }
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