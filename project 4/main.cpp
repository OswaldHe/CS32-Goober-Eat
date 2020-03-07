#include <iostream>
#include <list>
#include <functional>
#include "ExpandableHashMap.h"
#include "provided.h"
using namespace std;

int main(int argc, const char * argv[]) {
    StreetMap m;
    m.load("/Users/admin/Desktop/20Q2/CS\ 32\ projects/skeleton/mapdata.txt");
    GeoCoord p("34.0547778", "-118.4802585");
    GeoCoord q("34.0664194", "-118.3811961");
    list<StreetSegment> route;
    double distanceTravelled = 0.0;
    vector<StreetSegment> s;
    bool b = m.getSegmentsThatStartWith(p, s);
    if(b) cout << s[0].end.latitudeText<<" "<< s[0].end.longitudeText<< endl;
    PointToPointRouter router(&m);
    DeliveryResult d = router.generatePointToPointRoute(p, q, route, distanceTravelled);
    for(auto it = route.begin(); it!=route.end(); it++){
        cout << it->name << endl;
    }
    cout << distanceTravelled << endl;
    
}
