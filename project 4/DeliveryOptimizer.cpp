#include "provided.h"
#include <vector>
#include <time.h>
#include <cmath>
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
    
    /*
     This function calculate the overall crow distance for a specific order
     of delivery requests, from the starting point and back to the starting
     point.
     */
    double CalculatePathDis(vector<DeliveryRequest> potentialSol, const GeoCoord& start) const{
        double length = 0.0; //initialize path distance
        
        length += distanceEarthMiles(start, potentialSol[0].location)
        + distanceEarthMiles(start, (potentialSol.end()-1)->location);
        
        for (int i = 0; i < potentialSol.size()-1; i++) {
            length += distanceEarthMiles(potentialSol[i].location, potentialSol[i+1].location);
        }
        
        return length;
    }
};

DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
    m_streetMap = sm;
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

/*
 Travelling Saleman Problem: NP hard
 Exact optimization: BFS -- O(N!)
 Attempt: Simulated Annealing Approximation
 */
void DeliveryOptimizerImpl::optimizeDeliveryOrder(
    const GeoCoord& depot,
    vector<DeliveryRequest>& deliveries,
    double& oldCrowDistance,
    double& newCrowDistance) const
{
    oldCrowDistance = CalculatePathDis(deliveries, depot);
    
    srand((unsigned)time(nullptr));
    double t_init = 10000.0; //initial temperature
    double t_end = 1e-8; //final temperature: annealing finished
    double q = 0.95; //annealing coefficient
    size_t size = deliveries.size();
    int link = pow(size, 2); //repeating size for each temperature
    
    vector<DeliveryRequest> original;// original solution
    vector<DeliveryRequest> newSol = deliveries; // potential new solution
    double length1, length2, dl; //difference between path distance
    double r; // random number to decide whether keep the original solution
    while (t_init > t_end) {
        for(int i = 0; i < link; i++){
            original = newSol;
            //random numbers for swaping
            double r1 = ((double)rand())/(RAND_MAX+1.0);
            double r2 = ((double)rand())/(RAND_MAX+1.0);
            int a = (int)(size*r1);
            int b = (int)(size*r2);
            swap(newSol[a], newSol[b]);
            length1 = CalculatePathDis(original, depot);
            length2 = CalculatePathDis(newSol, depot);
            dl = length2 - length1;
            if(dl >= 0){//if the new path is longer
                r = ((double)rand())/(RAND_MAX);
                if(exp(-dl/t_init) <= r){ //keep original
                    newSol = original;
                }
            }
        }
        t_init*=q; //annealing
    }
    
    deliveries = newSol;
    newCrowDistance = CalculatePathDis(deliveries, depot);
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
