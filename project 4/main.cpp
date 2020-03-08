#include <iostream>
#include <list>
#include <functional>
#include "ExpandableHashMap.h"
#include "provided.h"
using namespace std;

int main(int argc, const char * argv[]) {
    StreetMap m;
    m.load("/Users/admin/Desktop/20Q2/CS\ 32\ projects/skeleton/mapdata.txt");
    GeoCoord p("34.0547000" , "-118.4794734"); // 10th Helena Drive
    GeoCoord q("34.0549825" , "-118.4795629"); //11 th
    GeoCoord r("34.0734335" , "-118.4449143"); //westwood 1
    GeoCoord w("34.0670755" , "-118.4451231"); //westwood 2
    GeoCoord e("34.0668846", "-118.4450991"); //westwood 3
    GeoCoord f("34.0558289" , "-118.4798296"); // 13th helena

    list<StreetSegment> route;
    double distanceTravelled = 0.0;
    vector<StreetSegment> s;
    bool b = m.getSegmentsThatStartWith(p, s);
    if(b) cout << s[0].end.latitudeText<<" "<< s[0].end.longitudeText<< endl;
    PointToPointRouter router(&m);
    router.generatePointToPointRoute(q, p, route, distanceTravelled);
//    for(auto it = route.begin(); it!=route.end(); it++){
//        cout << it->name << endl;
//    }
    cout << distanceTravelled << endl;
    
    DeliveryOptimizer del(&m);
    double oldDis, newDis;
    DeliveryRequest x("Chicken", q); // 1
    DeliveryRequest y("beer", r); //2
    DeliveryRequest z("pizza", w); //2
    DeliveryRequest a("burger", e); //2
    DeliveryRequest c("haha", f); //1

    
    vector<DeliveryRequest> deli;
    deli.push_back(a);
    deli.push_back(z);
    deli.push_back(x);
    deli.push_back(y);
    deli.push_back(c);
    del.optimizeDeliveryOrder(p, deli, oldDis, newDis);
    cout << oldDis << " "<< newDis <<" " << deli.size()<< endl;
    cout << deli[0].item << " " << deli[1].item << " " << deli[2].item << " " << deli[3].item << " " << deli[4].item <<endl;
    
    vector<DeliveryCommand> dc;
    DeliveryPlanner dp(&m);
    dp.generateDeliveryPlan(p, deli, dc, distanceTravelled);
    cout << distanceTravelled << endl;
    for(int i = 0; i < dc.size(); i++){
        cout << dc[i].description() << endl;
    }
    
}
