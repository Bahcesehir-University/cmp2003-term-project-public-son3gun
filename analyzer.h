#ifndef ANALYZER_H
#define ANALYZER_H

#include <string>
#include <vector>
#include <unordered_map>

struct ZoneCount {
    string zone;
    int count;
};

struct SlotCount {
    string zone;
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
    void processLine(const string& line);
    unordered_map<string, int> zone_counts;
    unordered_map<string, int> slot_counts;
};

#endif



