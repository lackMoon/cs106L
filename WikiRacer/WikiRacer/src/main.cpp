
#include <iostream>
#include <unordered_set>
#include <vector>
#include <queue>
#include <unordered_map>
#include "wikiscraper.h"


using std::cout;            using std::endl;
using std::string;          using std::vector;
using std::priority_queue;  using std::unordered_map;
using std::unordered_set;


/*
 * This is the function takes two string representing the
 * names of a start_page and end_page and is supposed to
 * return a ladder, represented as a vector<string>,
 * of links that can be followed from start_page to get
 * to the end_page.
 */
vector<string> findWikiLadder(const string& start_page, const string& end_page) {
    // creates WikiScraper object
    WikiScraper scraper;

    //creates visited pages set
    unordered_set<string> visitedPages{};
    visitedPages.insert(start_page);

    //Create a ladder containing {start_page}
    vector<string> ladder{start_page};

    // gets the set of links on page specified by end_page
    // variable and stores in target_set variable
    auto target_set = scraper.getLinkSet(end_page);

    // Comparison function for priority_queue
     auto cmpFn = [&scraper,&target_set](const vector<string>& a, const vector<string>& b) -> bool {
         string page1 = a.back();
         string page2 = b.back();
         int num1,num2;
         auto link_set1 = scraper.getLinkSet(page1);
         auto link_set2 = scraper.getLinkSet(page2);
         for(auto &page : target_set) {
             if (link_set1.count(page)) {
                 num1++;
             }
             if (link_set2.count(page)) {
                 num2++;
             }
         }
         return num1 < num2;
     };

     // creates a priority_queue names ladderQueue
     std::priority_queue<vector<string>, vector<vector<string>>,
      decltype(cmpFn)> ladderQueue(cmpFn);

     //add a ladder to the queue.
     ladderQueue.push(ladder);

     while (!ladderQueue.empty()) {
         //Dequeue the highest priority partial-ladder from the front of the queue.
         vector<string> currentLadder = ladderQueue.top();
         ladderQueue.pop();

         //Get the set of links of the current page
         auto current_set = scraper.getLinkSet(currentLadder.back());

         if (current_set.count(end_page)) {
             //Add end_page to the ladder you just dequeued and return it.
             currentLadder.push_back(end_page);
             return currentLadder;
         }

         for(auto& neighbour_page : current_set) {
             //If this neighbour page hasn’t already been visited
             if (!visitedPages.count(neighbour_page)) {
                 visitedPages.insert(neighbour_page);
                 //Create a copy of the current partial-ladder.
                 vector<string> newLadder{currentLadder};
                 //Put the neighbor page string on top of the copied ladder.
                 newLadder.push_back(neighbour_page);
                 //Add the copied ladder to the queue.
                 ladderQueue.push(newLadder);
             }
         }
     }
     return {};
}



int main() {
    auto ladder = findWikiLadder("Milkshake", "Gene");
    cout << endl;

    if(ladder.empty()) {
        cout << "No ladder found!" << endl;
    } else {
        cout << "Ladder found:" << endl;
        cout << "\t";
        for(auto start = ladder.begin(),end = ladder.end();start != end; start++) {
            if (start != ladder.begin()) {
                cout << " -> ";
            }
            cout << *start;
        }
        cout << endl;
    }

    return 0;
}




