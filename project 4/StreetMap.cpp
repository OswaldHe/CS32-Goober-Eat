#include "provided.h"
#include "ExpandableHashMap.h"
#include <string>
#include <vector>
#include <functional>
#include <fstream>
using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
private:
    ExpandableHashMap<GeoCoord, vector<StreetSegment>>* m_map;
};

StreetMapImpl::StreetMapImpl()
{
    m_map = new ExpandableHashMap<GeoCoord, vector<StreetSegment>>;
}

StreetMapImpl::~StreetMapImpl()
{
    delete m_map;
}

bool StreetMapImpl::load(string mapFile)
{
    ifstream mapData(mapFile);
    if(!mapData){
        cerr << "Cannot Load file" << endl;
        return false;
    }
    string line;
    while(getline(mapData, line)){
        string name = line;
        getline(mapData, line);
        int num = stoi(line);
        for(int i = 0; i < num; i++){
            string coord[4];
            getline(mapData, coord[0], ' ');
            getline(mapData, coord[1], ' ');
            getline(mapData, coord[2], ' ');
            getline(mapData, coord[3]);
            GeoCoord start(coord[0], coord[1]);
            GeoCoord end(coord[2], coord[3]);
            StreetSegment forward(start, end, name);
            StreetSegment reverse(end, start, name);
            vector<StreetSegment>* v = m_map->find(start);
            if(v){
                v->push_back(forward);
            }else{
                vector<StreetSegment> segments;
                segments.push_back(forward);
                m_map->associate(start, segments);
            }
            v = m_map->find(end);
            if (v) {
                v->push_back(reverse);
            } else {
                vector<StreetSegment> segments;
                segments.push_back(reverse);
                m_map->associate(end, segments);
            }
        }
        
    }
    return true;
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    vector<StreetSegment>* s;
    if((s = m_map->find(gc))){
        segs = *s;
        return true;
    }
    return false;
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}
