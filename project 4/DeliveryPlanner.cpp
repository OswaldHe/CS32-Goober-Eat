#include "provided.h"
#include <vector>
using namespace std;

class DeliveryPlannerImpl
{
public:
    DeliveryPlannerImpl(const StreetMap* sm);
    ~DeliveryPlannerImpl();
    DeliveryResult generateDeliveryPlan(
        const GeoCoord& depot,
        const vector<DeliveryRequest>& deliveries,
        vector<DeliveryCommand>& commands,
        double& totalDistanceTravelled) const;
private:
    const StreetMap* m_streetMap;
    DeliveryOptimizer* m_optimizer;
    PointToPointRouter* m_router;
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm)
{
    m_streetMap = sm;
    m_optimizer = new DeliveryOptimizer(sm);
    m_router = new PointToPointRouter(sm);
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
    delete m_optimizer;
    delete m_router;
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    vector<DeliveryRequest> optimizedDel = deliveries;
    double oldDis, newDis;
    double totalDistance = 0;
    m_optimizer->optimizeDeliveryOrder(depot, optimizedDel, oldDis, newDis);
    GeoCoord cur = depot;
    list<StreetSegment> route;
    double temp;
    commands.clear();
    for (int i = 0; i <= optimizedDel.size(); i++) {
        DeliveryResult r;
        if(i==optimizedDel.size()){
            r = m_router->generatePointToPointRoute(cur, depot, route, temp);
        }else{
            r = m_router->generatePointToPointRoute(cur, optimizedDel[i].location, route, temp);
        }
        if(r==BAD_COORD||r==NO_ROUTE)return r;
        totalDistance += temp;
        auto it = route.begin();
        while (it!=route.end()) {
            StreetSegment start = *it;
            string streetName = it->name;
            double angle = angleOfLine(*it);
            string direction;
            if(angle < 22.5) direction = "east";
            else if (angle < 67.5) direction = "northeast";
            else if (angle < 112.5) direction = "north";
            else if (angle < 157.5) direction = "northwest";
            else if (angle < 202.5) direction = "west";
            else if (angle < 247.5) direction = "southwest";
            else if (angle < 292.5) direction = "south";
            else if (angle < 337.5) direction = "southeast";
            else direction = "east";
            DeliveryCommand dc;
            dc.initAsProceedCommand(direction, streetName, 0);
            while (it->name==streetName&&it!=route.end()) {
                dc.increaseDistance(distanceEarthMiles(it->start, it->end));
                if(i!=optimizedDel.size()&& optimizedDel[i].location==it->end){
                    DeliveryCommand d;
                    d.initAsDeliverCommand(optimizedDel[i].item);
                    commands.push_back(d);
                }
                it++;
            }
            commands.push_back(dc);
            if(it==route.end())break;
            double turn = angleBetween2Lines(*it, start);
            if(turn>=1 && turn <=180){
                dc.initAsTurnCommand("left", it->name);
                commands.push_back(dc);
            }
            else if (turn >=180 && turn <=359){
                dc.initAsTurnCommand("right", it->name);
                commands.push_back(dc);
            }
        }
        if(i!=optimizedDel.size())
            cur = optimizedDel[i].location;
    }
    totalDistanceTravelled = totalDistance;
    return DELIVERY_SUCCESS;  
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
    m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
    delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
    const GeoCoord& depot,
    const vector<DeliveryRequest>& deliveries,
    vector<DeliveryCommand>& commands,
    double& totalDistanceTravelled) const
{
    return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
