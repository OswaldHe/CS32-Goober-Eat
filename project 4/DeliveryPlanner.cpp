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
    //generate optimized order for delivery
    m_optimizer->optimizeDeliveryOrder(depot, optimizedDel, oldDis, newDis);
    GeoCoord cur = depot;
    list<StreetSegment> route;
    double temp;
    commands.clear();
    for (int i = 0; i <= optimizedDel.size(); i++) {
        DeliveryResult r;
        if(i==optimizedDel.size()){ //move back to depot
            r = m_router->generatePointToPointRoute(cur, depot, route, temp);
        }else{//delivering
            r = m_router->generatePointToPointRoute(cur, optimizedDel[i].location, route, temp);
        }
        if(r==BAD_COORD||r==NO_ROUTE)return r;
        totalDistance += temp;
        list<StreetSegment>::iterator it = route.begin();
        while (it!=route.end()) {
            /*
             Generate delivery command for each street
             */
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
            double isDelivery = false;
            //track the length of a street
            while (it!=route.end()&&it->name==streetName) {
                dc.increaseDistance(distanceEarthMiles(it->start, it->end));
                // if there is a delivery in this street
                // push the proceed command, then delivery command, then continue to next loop
                if(i!=optimizedDel.size()&& optimizedDel[i].location==it->end){
                    DeliveryCommand d;
                    d.initAsDeliverCommand(optimizedDel[i].item);
                    commands.push_back(dc);
                    commands.push_back(d);
                    isDelivery = true;
                    it++;
                    break;
                }
                it++;
            }
            if (isDelivery) {
                continue;
            }
            commands.push_back(dc);
            if(it==route.end())break;
            // check whether to turn
            list<StreetSegment>::iterator it2 = it;
            it2--;
            double turn = angleBetween2Lines(*(it2), *it);
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
