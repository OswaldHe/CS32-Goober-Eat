#include "provided.h"
#include <list>
#include <map>
using namespace std;


class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
private:
    struct Loc{
        Loc(GeoCoord l, double f=0.0, double g=0.0, double h=0.0, Loc* p = nullptr):location(l), m_f(f), m_g(g), m_h(h), parent(p){}
        bool operator<(const Loc& a){
            return m_f<a.m_f;
        }
        GeoCoord location;
        double m_f, m_g, m_h;
        Loc* parent;
    };
    const StreetMap* m_streetMap;
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    m_streetMap = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(const GeoCoord& start, const GeoCoord& end, list<StreetSegment>& route, double& totalDistanceTravelled) const{
    vector<StreetSegment> v;
    if (!m_streetMap->getSegmentsThatStartWith(start, v)) {
        return BAD_COORD;
    }
    if (!m_streetMap->getSegmentsThatStartWith(end, v)) {
        return BAD_COORD;
    }
    
    bool found = false;
    map<GeoCoord, Loc*> openList;
    map<GeoCoord, Loc*> closedList;
    openList[start] =  new Loc(start, distanceEarthMiles(start, end), 0, distanceEarthMiles(start, end));;
    Loc* cur;
    while(openList.size()!=0){
        cur = openList.begin()->second;
        for (auto it = openList.begin(); it!=openList.end(); it++) {
            if(it->second->m_f<cur->m_f){
                cur = it->second;
            }
        }
        openList.erase(openList.find(cur->location));
        if(cur->location==end){
            found = true;
            break;
        }
        vector<StreetSegment> segs;
        m_streetMap->getSegmentsThatStartWith(cur->location, segs);
        for(int i = 0; i < segs.size(); i++){
            GeoCoord landmark = segs[i].end;
            double currentCost = cur->m_g+distanceEarthMiles(cur->location, landmark);
            if(openList.find(landmark)!=openList.end()){
                Loc* t = openList[landmark];
                if(t->m_g<=currentCost)continue;
            }else if (closedList.find(landmark)!=closedList.end()){
                Loc* t = closedList[landmark];
                if(t->m_g<=currentCost)continue;
                openList[landmark] = t;
                auto it = closedList.find(landmark);
                closedList.erase(it);
            }else{
                double h = distanceEarthMiles(landmark, end);
                Loc* t = new Loc(landmark, h, 0, h);
                openList[landmark] = t;
            }
            openList[landmark]->m_g = currentCost;
            openList[landmark]->m_f = currentCost+openList[landmark]->m_h;
            openList[landmark]->parent = cur;
        }
        closedList[cur->location] = cur;
    }
    if(found==true){
        totalDistanceTravelled = cur->m_g;
        while(cur->parent != nullptr){
            vector<StreetSegment> s;
            if(m_streetMap->getSegmentsThatStartWith(cur->parent->location, s)){
                for(int i = 0; i < s.size(); i++){
                    if(s[i].end==cur->location){
                        route.push_front(s[i]);
                        break;
                    }
                }
            }
            cur = cur->parent;
        }
        return DELIVERY_SUCCESS;
    }
    return NO_ROUTE;  // Delete this line and implement this function correctly
}

//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
