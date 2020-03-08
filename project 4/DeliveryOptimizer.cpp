#include "provided.h"
#include <vector>
using namespace std;

class DeliveryOptimizerImpl
{
public:
    DeliveryOptimizerImpl(const StreetMap* sm);
    ~DeliveryOptimizerImpl();
    void optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const;
private:
    const StreetMap* m_streetMap;
    struct Node{
        Node(DeliveryRequest d, bool v):m_d(d),m_visited(v){}
        DeliveryRequest m_d;
        bool m_visited;
    };
    vector<DeliveryRequest> bestPath(DeliveryRequest start, DeliveryRequest end, vector<Node*> old, double& path) const;
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
    m_streetMap = sm;
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    GeoCoord cur = depot;
    oldCrowDistance = 0;
    vector<Node*> nodes;
    for (int i = 0; i < deliveries.size(); i++) {
        oldCrowDistance+=distanceEarthMiles(cur, deliveries[i].location);
        nodes.push_back(new Node(deliveries[i], false));
        cur = deliveries[i].location;
    }
    oldCrowDistance+=distanceEarthMiles(depot, cur);
    //nodes.push_back(new Node(deliveries[deliveries.size()-1], false));
    double shortestPath;
    DeliveryRequest del("starting",depot);
    nodes.push_back(new Node(del, false));
    deliveries = bestPath(del, del, nodes, shortestPath);
    deliveries.erase(deliveries.begin());
    deliveries.erase(deliveries.end()-1);
    newCrowDistance = shortestPath;
}

vector<DeliveryRequest> DeliveryOptimizerImpl::bestPath(DeliveryRequest start, DeliveryRequest end, vector<Node*> old, double& path) const{
    vector<DeliveryRequest> v;
    bool allVisit = true;
    v.push_back(start);
    for(int i = 0; i < old.size(); i++){
//        if(old[i]->m_d.location == start.location){
//            old[i]->m_visited = true;
//            continue;
//        }
        if(!old[i]->m_visited) {
            allVisit = false;
            break;
        }
    }
    
    if(allVisit){
        path = distanceEarthMiles(end.location, start.location);
        return v;
    }
    double length = 100000000;
//    int index = 0;

    for(int i = 0; i < old.size(); i++){
        double temp = 0;
        if(old[i]->m_d.location!=start.location&&old[i]->m_visited==false){
            old[i]->m_visited = true;
            vector<DeliveryRequest> t = bestPath(old[i]->m_d, end, old, temp);
            temp+=distanceEarthMiles(start.location, old[i]->m_d.location);
            if(length > temp) {
                length = temp;
                v = t;
            }
            old[i]->m_visited = false;
        }
    }
    path = length;
//    old[index]->m_visited = true;
//    v = bestPath(old[index]->m_d, end, old, length);
    v.insert(v.begin(), start);
    return v;
}

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
