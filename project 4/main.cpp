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
    vector<StreetSegment> s;
    bool b = m.getSegmentsThatStartWith(p, s);
    if(b) cout << s[0].end.latitudeText<<" "<< s[0].end.longitudeText<< endl;
}
