#ifndef ANALYZER_H
#define ANALYZER_H

#include <string>
#include <vector>
#include <unordered_map>

struct ZoneCount {
    std::string zone;
    int count;
};

struct SlotCount {
    std::string zone;
    int hour;
    int count;
};

class TripAnalyzer {
public:
    void ingestFile(const std::string& csvPath);
    void ingestStdin();
    std::vector<ZoneCount> topZones(int k = 10) const;
    std::vector<SlotCount> topBusySlots(int k = 10) const;

private:
    void processLine(const std::string& line);
    std::unordered_map<std::string, int> zone_counts;
    std::unordered_map<std::string, int> slot_counts;
};

#endif
