#include "GeohashHelper.h"
#include "ns3/mobility-module.h"
#include "ns3/log.h"

#include <cmath>
#include <bitset>
#include <iostream>

NS_LOG_COMPONENT_DEFINE("GeohashHelper");

GeohashHelper& GeohashHelper::GetInstance()
{
    static GeohashHelper instance(0.0, 2000.0, 0.0, 1400.0, 4);
    return instance;
}

GeohashHelper::GeohashHelper(double minX, double maxX, double minY, double maxY, int precision)
    : m_minX(minX), m_maxX(maxX), m_minY(minY), m_maxY(maxY), m_precision(precision)
{
}

uint64_t GeohashHelper::Encode(double x, double y)
{
    // Normalize x and y to be between 0 and 1
    double normalizedX = (x - m_minX) / (m_maxX - m_minX);
    double normalizedY = (y - m_minY) / (m_maxY - m_minY);

    //NS_LOG_UNCOND("Normalized X: " << normalizedX << ", Normalized Y: " << normalizedY);

    // Convert to geohash using interleaving bits
    uint64_t geohash = 0;
    for (int i = 0; i < m_precision; ++i)
    {
        normalizedX *= 2;
        normalizedY *= 2;

        int bitX = static_cast<int>(normalizedX);
        int bitY = static_cast<int>(normalizedY);

        // Remove the integer part after calculating the bit
        normalizedX -= bitX;
        normalizedY -= bitY;

        //NS_LOG_UNCOND("Iteration " << i << ": bitX = " << bitX << ", bitY = " << bitY);

        // Interleave the bits
        geohash |= (bitX << (2 * i + 1)) | (bitY << (2 * i));
        //NS_LOG_UNCOND("Geohash after iteration " << i << ": " << geohash);
    }
    return geohash;
}

void GeohashHelper::PrintGeohash(Ptr<Node> node)
{
    Ptr<MobilityModel> mobility = node->GetObject<MobilityModel>();
    if (mobility == nullptr)
    {
        NS_LOG_ERROR("Mobility model not found for node " << node->GetId());
        return;
    }

    Vector position = mobility->GetPosition();
    uint64_t geohash = Encode(position.x, position.y);
    NS_LOG_UNCOND("Node " << node->GetId() << " Position: (x: " << position.x << ", y: " << position.y << ") Geohash: " << geohash);
}

int GeohashHelper::GetHammingDistance(uint64_t geohash1, uint64_t geohash2)
{
    uint64_t difference = geohash1 ^ geohash2;
    return __builtin_popcountll(difference);  // Return the number of differing bits
}
