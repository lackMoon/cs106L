#include <iostream>
#include <fstream>
#include <unordered_set>
#include <algorithm>

#define PREFIX_LEN 15
using std::cout;            using std::endl;
using std::string;          using std::unordered_set;

/*
 * Note that you have to pass in the file as a single string
 * into the findWikiLinks function!
 * So we have to turn the passed-in file into a single string...
 * does that sound familiar at all?
 */
unordered_set<string> findWikiLinks(const string& page_html) {
    unordered_set<string> link_set;
    string start_token = "<a href=\"/wiki/";
    char end_token = '"';
    auto start = page_html.begin();
    auto end = page_html.end();
    while (start != end) {
        string valid_string = "";
        auto link_start = std::search(start, end, start_token.begin(), start_token.end());
        if(link_start == end) break;
        link_start += PREFIX_LEN;
        auto link_end = std::find(link_start, end, end_token);
        while(link_start != link_end) {
            valid_string += *link_start;
            link_start++;
        }
        if (std::all_of(valid_string.begin(), valid_string.end(), [](char ch) { return ch != '#' && ch != ':'; })) {
            link_set.insert(valid_string);
        }
        start = link_start;
    }
    return link_set;
}

string fileToString(std::ifstream& inp) {
    string ret;
    string line;
    while(std::getline(inp, line)) {
        ret += line + " ";
    }
    return ret;
}

void printSet(unordered_set<string> set) {
    int i{1};
    for(auto start = set.begin(),end = set.end();start != end; start++) {
        cout << i << "." << *start << endl;
        i++;
    }
}

int main() {
    /* Note if your file reading isn't working, please go to the
     * projects tab on the panel on the left, and in the run section,
     * uncheck the "Run in terminal" box and re-check it. This
     * should fix things.
     */

    cout << "Enter a file name: ";
    string filename,page;
    getline(std::cin, filename);
    std::ifstream input(filename);
    if(!input.is_open()) {
        std::cerr << "Cannot found the file" << filename << endl;
        return -1;
    }
    string data = fileToString(input);
    unordered_set<string> link_set = findWikiLinks(data);
    printSet(link_set);

}

