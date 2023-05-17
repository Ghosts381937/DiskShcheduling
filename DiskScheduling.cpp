#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

struct ResultOfDiskScheduling {
    vector<int> pathOfHeadMovement;
    int pathLength;
};

vector<int> spilt(string str, char delim) {
    stringstream ss(str);
    vector<int> result;
    while(ss.good()) {
        string substr;
        getline(ss, substr, delim);
        result.push_back(stoi(substr));
    }
    result.pop_back();
    return result;
}

bool readJob(fstream &file, string &mode, int &totalTracks, int &currentHead, int &direction, vector<int> &trackRequests) {
    string buffer;
    
    if(!getline(file, buffer) || buffer.empty()) {
        return false;//There are not any jobs in the file
    }

    mode = buffer;
    getline(file, buffer);
    totalTracks = stoi(buffer);
    getline(file, buffer);
    currentHead = stoi(buffer);
    getline(file, buffer);
    direction = stoi(buffer);
    getline(file, buffer);
    trackRequests = spilt(buffer, ',');
    return true;
}

int moveToNextHead(int currentHead, int nextHead, ResultOfDiskScheduling &resultOfDiskScheduling) {
    resultOfDiskScheduling.pathLength += abs(currentHead - nextHead);
    currentHead = nextHead;
    resultOfDiskScheduling.pathOfHeadMovement.push_back(currentHead);
    return currentHead;
}

vector<int>::iterator spiltTrackRequests(int currentHead, int direction, vector<int> &trackRequests) {
    vector<int>::iterator it;
    if(direction == 0) {
        it = trackRequests.begin();
        for(it; it != trackRequests.end(); it++) {
            if(*it > currentHead) {
                break;
            } 
        }
    }
    else if(direction == 1) {
        it = trackRequests.end() - 1;
        for(it; it != trackRequests.begin() - 1; it--) {
            if(*it < currentHead) {
                break;
            } 
        }
    }
    return it;
}

ResultOfDiskScheduling fcfs(int currentHead, vector<int> &trackRequests) {
    ResultOfDiskScheduling result;
    result.pathLength = 0;
    result.pathOfHeadMovement.push_back(currentHead);
    for(int nextHead : trackRequests) {
        currentHead = moveToNextHead(currentHead, nextHead, result);
    }
    return result;
}

ResultOfDiskScheduling sstf(int currentHead, vector<int> &trackRequests) {
    //initialize
    ResultOfDiskScheduling result;
    result.pathLength = 0;
    result.pathOfHeadMovement.push_back(currentHead);

    while(!trackRequests.empty()) {
        int nextHeadIndex = 0;
        int nextHead = trackRequests[nextHeadIndex];
        int minDistance = abs(currentHead - nextHead);
        int distance = 0;

        //find the nextHead with the least distance
        for(int i = 0; i < trackRequests.size(); i++) {
            distance = abs(currentHead - trackRequests[i]);
            if(distance < minDistance) {
                nextHeadIndex = i;
                nextHead = trackRequests[nextHeadIndex];
                minDistance = distance;
            }
        }

        currentHead = moveToNextHead(currentHead, nextHead, result);

        //delete the single request that had been finished
        trackRequests[nextHeadIndex] = trackRequests.back();
        trackRequests.pop_back();
    }
    return result;
}

ResultOfDiskScheduling scan(int totalTracks, int currentHead, int direction, vector<int> &trackRequests) {
    //initialize
    sort(trackRequests.begin(), trackRequests.end());
    ResultOfDiskScheduling result;
    result.pathLength = 0;
    result.pathOfHeadMovement.push_back(currentHead);
    //spilt trackRequests to left part and right part
    vector<int>::iterator mid = spiltTrackRequests(currentHead, direction, trackRequests);
    vector<int> leftTracks(trackRequests.begin(), (direction == 0) ? mid : mid + 1);
    vector<int> rightTracks((direction == 0) ? mid : mid + 1, trackRequests.end());
    
    switch (direction) {
        case 0:
            for(int i = 0; i < rightTracks.size(); i++) {
                currentHead = moveToNextHead(currentHead, rightTracks[i], result);
            }
            if(leftTracks.size() > 0) {
                currentHead = moveToNextHead(currentHead, totalTracks - 1, result);
            }
            for(int i = leftTracks.size() - 1; i >= 0; i--) {
                currentHead = moveToNextHead(currentHead, leftTracks[i], result);
            }
            break;
        case 1:
            for(int i = leftTracks.size() - 1; i >= 0; i--) {
                currentHead = moveToNextHead(currentHead, leftTracks[i], result);
            }
            if(rightTracks.size() > 0) {
                currentHead = moveToNextHead(currentHead, 0, result);
            }
            for(int i = 0; i < rightTracks.size(); i++) {
                currentHead = moveToNextHead(currentHead, rightTracks[i], result);
            }
            break;
    }

    return result;
}

ResultOfDiskScheduling cscan(int totalTracks, int currentHead, int direction, vector<int> &trackRequests) {
    //initialize
    sort(trackRequests.begin(), trackRequests.end());
    ResultOfDiskScheduling result;
    result.pathLength = 0;
    result.pathOfHeadMovement.push_back(currentHead);
    //spilt trackRequests to left part and right part
    vector<int>::iterator mid = spiltTrackRequests(currentHead, direction, trackRequests);
    vector<int> leftTracks(trackRequests.begin(), (direction == 0) ? mid : mid + 1);
    vector<int> rightTracks((direction == 0) ? mid : mid + 1, trackRequests.end());
    
    switch (direction) {
        case 0:
            for(int i = 0; i < rightTracks.size(); i++) {
                currentHead = moveToNextHead(currentHead, rightTracks[i], result);
            }
            if(leftTracks.size() > 0) {
                currentHead = moveToNextHead(currentHead, totalTracks - 1, result);
                currentHead = moveToNextHead(currentHead, 0, result);
            }
            for(int i = 0; i < leftTracks.size(); i++) {
                currentHead = moveToNextHead(currentHead, leftTracks[i], result);
            }
            break;
        case 1:
            for(int i = leftTracks.size() - 1; i >= 0; i--) {
                currentHead = moveToNextHead(currentHead, leftTracks[i], result);
            }
            if(rightTracks.size() > 0) {
                currentHead = moveToNextHead(currentHead, 0, result);
                currentHead = moveToNextHead(currentHead, totalTracks - 1, result);
            }
            for(int i = rightTracks.size() - 1; i >= 0; i--) {
                currentHead = moveToNextHead(currentHead, rightTracks[i], result);
            }
            break;
    }

    return result;
}

ResultOfDiskScheduling look(int totalTracks, int currentHead, int direction, vector<int> &trackRequests) {
    //initialize
    sort(trackRequests.begin(), trackRequests.end());
    ResultOfDiskScheduling result;
    result.pathLength = 0;
    result.pathOfHeadMovement.push_back(currentHead);
    //spilt trackRequests to left part and right part
    vector<int>::iterator mid = spiltTrackRequests(currentHead, direction, trackRequests);
    vector<int> leftTracks(trackRequests.begin(), (direction == 0) ? mid : mid + 1);
    vector<int> rightTracks((direction == 0) ? mid : mid + 1, trackRequests.end());

    switch (direction) {
        case 0:
            for(int i = 0; i < rightTracks.size(); i++) {
                currentHead = moveToNextHead(currentHead, rightTracks[i], result);
            }
            for(int i = leftTracks.size() - 1; i >= 0; i--) {
                currentHead = moveToNextHead(currentHead, leftTracks[i], result);
            }
            break;
        case 1:
            for(int i = leftTracks.size() - 1; i >= 0; i--) {
                currentHead = moveToNextHead(currentHead, leftTracks[i], result);
            }
            for(int i = 0; i < rightTracks.size(); i++) {
                currentHead = moveToNextHead(currentHead, rightTracks[i], result);
            }
            break;
    }

    return result;
}

ResultOfDiskScheduling clook(int totalTracks, int currentHead, int direction, vector<int> &trackRequests) {
    //initialize
    sort(trackRequests.begin(), trackRequests.end());
    ResultOfDiskScheduling result;
    result.pathLength = 0;
    result.pathOfHeadMovement.push_back(currentHead);
    
    vector<int>::iterator mid = spiltTrackRequests(currentHead, direction, trackRequests);
    vector<int> leftTracks(trackRequests.begin(), (direction == 0) ? mid : mid + 1);
    vector<int> rightTracks((direction == 0) ? mid : mid + 1, trackRequests.end());

    switch (direction) {
        case 0:
            for(int i = 0; i < rightTracks.size(); i++) {
                currentHead = moveToNextHead(currentHead, rightTracks[i], result);
            }
            for(int i = 0; i < leftTracks.size(); i++) {
                currentHead = moveToNextHead(currentHead, leftTracks[i], result);
            }
            break;
        case 1:
            for(int i = leftTracks.size() - 1; i >= 0; i--) {
                currentHead = moveToNextHead(currentHead, leftTracks[i], result);
            }
            for(int i = rightTracks.size() - 1; i >= 0; i--) {
                currentHead = moveToNextHead(currentHead, rightTracks[i], result);
            }
            break;
    }

    return result;
}

void output(ResultOfDiskScheduling &result) {
    cout << result.pathOfHeadMovement[0];
    for(int i = 1; i < result.pathOfHeadMovement.size(); i++) {
        cout << "->" << result.pathOfHeadMovement[i];
    }
    cout << endl;
    printf("Move %d Tracks\n\n", result.pathLength);
}

int main() {
    fstream file;
    string mode;
    int totalTracks;
    int currentHead;
    int direction;
    vector<int> trackRequests;
    ResultOfDiskScheduling resultOfDiskScheduling;

    file.open("diskJob.txt");

    while(readJob(file, mode, totalTracks, currentHead, direction, trackRequests)) {
        if(mode == "FCFS") {
            resultOfDiskScheduling = fcfs(currentHead, trackRequests);
        }
        else if(mode == "SSTF") {
            resultOfDiskScheduling = sstf(currentHead, trackRequests);
        }
        else if(mode == "SCAN") {
            resultOfDiskScheduling = scan(totalTracks, currentHead,direction, trackRequests);
        }
        else if(mode == "CSCAN") {
            resultOfDiskScheduling = cscan(totalTracks, currentHead,direction, trackRequests);
        }
        else if(mode == "LOOK") {
            resultOfDiskScheduling = look(totalTracks, currentHead,direction, trackRequests);
        }
        else if(mode == "CLOOK") {
            resultOfDiskScheduling = clook(totalTracks, currentHead,direction, trackRequests);
        }

        output(resultOfDiskScheduling);
    }
}