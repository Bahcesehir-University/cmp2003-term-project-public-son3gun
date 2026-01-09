#include "analyzer.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>
#include <iostream>

static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

void TripAnalyzer::ingestFile(const std::string& csvPath) {
    zone_counts.clear();
    slot_counts.clear();

    std::ifstream file(csvPath);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        processLine(line);
    }
}

void TripAnalyzer::ingestStdin() {
    zone_counts.clear();
    slot_counts.clear();

    std::string line;
    bool first = true;

    while (std::getline(std::cin, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();

        if (first) {
            std::string t = trim(line);
            if (t.find("TripID") != std::string::npos) {
                first = false;
                continue;
            }
        }
        first = false;

        processLine(line);
    }
}

void TripAnalyzer::processLine(const std::string& line) {
    std::string cleaned = trim(line);
    if (cleaned.empty()) return;

    std::stringstream ss(cleaned);
    std::string segment;
    std::vector<std::string> parts;

    while (std::getline(ss, segment, ',')) {
        parts.push_back(segment);
    }

    if (!(parts.size() == 3 || parts.size() >= 6)) return;

    std::string zone;
    std::string timeStr;

    if (parts.size() >= 6) {
        zone = parts[1];
        timeStr = parts[3];
    } else {
        zone = parts[1];
        timeStr = parts[2];
    }

    zone = trim(zone);
    timeStr = trim(timeStr);

    if (zone.empty() || timeStr.empty()) return;

    int hour = -1;
    size_t spacePos = timeStr.find(' ');
    if (spacePos != std::string::npos && spacePos + 3 < timeStr.size()) {
        char a = timeStr[spacePos + 1];
        char b = timeStr[spacePos + 2];

        if (a >= '0' && a <= '9' && b >= '0' && b <= '9') {
            hour = (a - '0') * 10 + (b - '0');
            if (timeStr[spacePos + 3] != ':') {
                hour = -1;
            }
        }
    }

    if (hour < 0 || hour > 23) return;

    zone_counts[zone]++;
    slot_counts[zone + "_" + std::to_string(hour)]++;
}

std::vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    if (k <= 0) return {};

    std::vector<ZoneCount> result;
    result.reserve(zone_counts.size());

    for (auto const& [zone, count] : zone_counts) {
        result.push_back({zone, count});
    }

    std::sort(result.begin(), result.end(), [](const ZoneCount& a, const ZoneCount& b) {
        if (a.count != b.count) return a.count > b.count;
        return a.zone < b.zone;
    });

    if (result.size() > (size_t)k) result.resize(k);
    return result;
}

std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    if (k <= 0) return {};

    std::vector<SlotCount> result;
    result.reserve(slot_counts.size());

    for (auto const& [key, count] : slot_counts) {
        size_t u = key.find_last_of('_');
        if (u == std::string::npos) continue;

        std::string zone = key.substr(0, u);
        std::string hourStr = key.substr(u + 1);

        int hour = 0;
        if (hourStr.empty()) continue;

        bool ok = true;
        for (char ch : hourStr) {
            if (ch < '0' || ch > '9') { ok = false; break; }
            hour = hour * 10 + (ch - '0');
        }
        if (!ok || hour < 0 || hour > 23) continue;

        result.push_back({zone, hour, count});
    }

    std::sort(result.begin(), result.end(), [](const SlotCount& a, const SlotCount& b) {
        if (a.count != b.count) return a.count > b.count;
        if (a.zone != b.zone) return a.zone < b.zone;
        return a.hour < b.hour;
    });

    if (result.size() > (size_t)k) result.resize(k);
    return result;
}



