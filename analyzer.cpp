#include "analyzer.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <vector>

using namespace std;

static string trim(const string& s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == string::npos) return "";
    size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

void TripAnalyzer::ingestStdin() {
    zone_counts.clear();
    slot_counts.clear();

    string line;
    bool first = true;

    while (getline(cin, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (first) {
            string t = trim(line);
            if (t.find("TripID") != string::npos) {
                first = false;
                continue;
            }
        }
        first = false;

        processLine(line);
    }
}

void TripAnalyzer::processLine(const string& line) {
    string cleaned = trim(line);
    if (cleaned.empty()) return;

    stringstream ss(cleaned);
    vector<string> parts;
    string seg;

    while (getline(ss, seg, ',')) {
        parts.push_back(seg);
    }

    if (!(parts.size() == 3 || parts.size() >= 6)) return;

    string zone = trim(parts[1]);
    string timeStr = (parts.size() >= 6) ? trim(parts[3]) : trim(parts[2]);

    if (zone.empty() || timeStr.empty()) return;

    int hour = -1;
    size_t sp = timeStr.find(' ');
    if (sp != string::npos && sp + 3 < timeStr.size()) {
        char a = timeStr[sp + 1];
        char b = timeStr[sp + 2];
        if (a >= '0' && a <= '9' && b >= '0' && b <= '9') {
            hour = (a - '0') * 10 + (b - '0');
            if (timeStr[sp + 3] != ':') hour = -1;
        }
    }

    if (hour < 0 || hour > 23) return;

    zone_counts[zone]++;
    slot_counts[zone + "_" + to_string(hour)]++;
}

vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    vector<ZoneCount> v;
    for (auto& p : zone_counts)
        v.push_back({p.first, p.second});

    sort(v.begin(), v.end(), [](auto& a, auto& b) {
        if (a.count != b.count) return a.count > b.count;
        return a.zone < b.zone;
    });

    if ((int)v.size() > k) v.resize(k);
    return v;
}

vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    vector<SlotCount> v;

    for (auto& p : slot_counts) {
        size_t u = p.first.find_last_of('_');
        if (u == string::npos) continue;

        string zone = p.first.substr(0, u);
        int hour = stoi(p.first.substr(u + 1));
        if (hour < 0 || hour > 23) continue;

        v.push_back({zone, hour, p.second});
    }

    sort(v.begin(), v.end(), [](auto& a, auto& b) {
        if (a.count != b.count) return a.count > b.count;
        if (a.zone != b.zone) return a.zone < b.zone;
        return a.hour < b.hour;
    });

    if ((int)v.size() > k) v.resize(k);
    return v;
}

