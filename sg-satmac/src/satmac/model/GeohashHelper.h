// GeohashHelper.h
#ifndef GEOHASH_HELPER_H
#define GEOHASH_HELPER_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"

using namespace ns3;

class GeohashHelper
{
public:
    // Get the singleton instance of GeohashHelper
    static GeohashHelper& GetInstance();

    uint64_t Encode(double x, double y);
    void PrintGeohash(Ptr<Node> node);
    int GetHammingDistance(uint64_t geohash1, uint64_t geohash2);

private:
    // Private constructor for singleton pattern
    GeohashHelper(double minX, double maxX, double minY, double maxY, int precision);

    double m_minX;
    double m_maxX;
    double m_minY;
    double m_maxY;
    int m_precision;
};

#endif // GEOHASH_HELPER_H
