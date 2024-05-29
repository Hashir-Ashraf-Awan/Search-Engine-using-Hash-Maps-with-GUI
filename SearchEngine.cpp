#include <iostream>
#include <string>
#include <iterator>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <map>
#include <set>
#include <sstream>
#include <dirent.h>

using namespace std;

class SearchEngine
{
    private:
    //unordered_map <string, int> ParsedWord;
    map <string, map <string, int> > ParsedWord;
    vector <string> QueryWords;
    vector <string> boolWords;
    vector < set <string> > QueryResults;

    public:
    SearchEngine()
    {
        cout << "                             Welcome to SearchEngine!\n\n                           ";
        cout << endl;
    }

    void Clear()
    {
        ParsedWord.clear();
        QueryWords.clear();
        boolWords.clear();
        QueryResults.clear();
    }

    void ParseFile(const string &filePath)
    {
        ifstream file(filePath);

        if (file.is_open())
        {
            string line;
            map <string, int> wordCount;

            while (getline(file, line))
            {
                istringstream iss(line);
                string word;

                while (iss >> word)
                {
                    transform(word.begin(), word.end(), word.begin(), ::tolower);

                    if (ParsedWord.find(word) == ParsedWord.end())
                    {
                        wordCount[word] = 1;
                    }
                    else
                    {
                        wordCount[word]++;
                    }
                }
            }

            ParsedWord[filePath] = wordCount;

        }
        else
        {
            cout << "Error: File not found" << endl;
        }
    }   

    void ReadFolderandParse(const string &folderPath)
    {
        DIR *directory;
        struct dirent *entry;

        directory = opendir(folderPath.c_str());

        if(directory == NULL)
        {
            cout << "Error: Directory not found" << endl;
            return;
        }
        else
        {
            while ((entry = readdir(directory)) != NULL)
            {
                string filename = entry->d_name;

                if (filename == "." || filename == "..")
                {
                    continue;
                }

                string filePath = folderPath + "/" + filename;

                ParseFile(filePath);

                //doc.push_back(filename);

                cout << "File " << filename << " parsed successfully." << endl;
            }
        }

        closedir(directory);
    }

    /*void Display()
    {
        //unordered_map <string, int> :: iterator it; 
        map< string, int> :: iterator it;
        it = ParsedWord.begin();
        vector <string> :: iterator it2;
        it2 = doc.begin();

        for (it2 = doc.begin(); it2 != doc.end(); ++it2)
        {
            cout << "\nthe document is: " << *it2 << endl << endl;
            for (it = ParsedWord.begin(); it != ParsedWord.end(); ++it)
            {
                cout << "the word is: " << "'" << it->first << "'" << " with index: " << it->second << " " << endl;
            }
        }
    }*/

    void Display()
    {
        map <string, map <string, int > > :: iterator it;
        it = ParsedWord.begin();

        map <string, int> :: iterator it2;
        it2 = it->second.begin();

        for (it = ParsedWord.begin(); it != ParsedWord.end(); ++it)
        {
            cout << "\nthe document is: " << it->first;
            cout << endl << endl;

            for (it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            {
                cout << "the word is: " << "'" << it2->first << "'" << " with count: " << it2->second << " ";
                cout << endl;
            }
        };
    }

    void ParseUserQuery(const string &query)
    {
        istringstream userQuery(query);
        string queryWord;

        if (query.empty())
        {
            cout << "Error: Query is empty" << endl;
            return;
        }
        else
        {
            while (userQuery >> queryWord)
            {
                if (queryWord == "AND")
                {
                    boolWords.push_back(queryWord);
                }
                else if (queryWord == "OR")
                {
                    boolWords.push_back(queryWord);
                }
                else
                {
                    QueryWords.push_back(queryWord);
                }
            }
        }
    }

    void SearchQuery()
    {
        for (int i = 0; i < QueryWords.size(); ++i)
        {
            set < string > storeWords;
            string word = QueryWords[i];

            map <string, map <string, int> > :: iterator it;
            //it = ParsedWord.find(word);

            for (it = ParsedWord.begin(); it != ParsedWord.end(); ++it)
            {
                if (it->second.find(word) != it->second.end())
                {
                    storeWords.insert(it->first);
                }
            }

            QueryResults.push_back(storeWords);
        }
    }

    void booleanLogic()
    {
        if (boolWords.empty())
        {
            return; 
        }
        else
        {
            for (int i = 0; i < boolWords.size(); ++i)
            {
                if (boolWords[i] == "AND")
                {
                    AND();
                }
                else if (boolWords[i] == "OR")
                {
                    OR();
                }
            }
        }
    }

    void AND()
    {
        if (QueryResults.size() < 2)
        {
            return;
        }

        set < string > set1 = QueryResults[0];
        set < string > setResult;

        for (int i = 1; i < QueryResults.size(); ++i)
        {
            set_intersection(set1.begin(), set1.end(), QueryResults[i].begin(), QueryResults[i].end(), inserter(setResult, setResult.begin()));

            set1 = setResult;
            setResult.clear();
        }

        QueryResults.clear();
        
        QueryResults.push_back(set1);
    }

    void OR()
    {
        if (QueryResults.size() < 2)
        {
            return;
        }

        set < string > set1 = QueryResults[0];
        set < string > setResult;

        for (int i = 1; i < QueryResults.size(); ++i)
        {
            set_union(QueryResults[i].begin(), QueryResults[i].end(), set1.begin(), set1.end(), inserter(setResult, setResult.begin()));

            set1 = setResult;
            setResult.clear();
        }

        QueryResults.clear();

        QueryResults.push_back(set1);
    }

    void DisplayQueryResult()
    {
        bool found = false;

        set < string > :: iterator it;

        for (int i = 0; i < QueryResults.size(); ++i)
        {
            if (!QueryResults[i].empty())
            {
                found = true;

                cout << "The document containing the query " << "'" << QueryWords[i] << "'" << " is: " << endl;
                cout << "\n";

                for (it = QueryResults[i].begin(); it != QueryResults[i].end(); ++it)
                {
                    cout << *it;
                    cout << endl;
                }
                
                cout << endl;
            }   
        }   

        if (!found)
        {
            cout << "No document found" << endl;
        }
    }

};

int main()
{
    SearchEngine engine;

    engine.Clear();

    engine.ReadFolderandParse("/Users/macbook/Documents/NUST 22-26/NUST Semester 04/DS/DSA Documents");

    engine.Display();
    cout << endl;

    int choice;
    choice = 0;

    while (true)
    {
        cout << "Enter your query: ";
        cout << endl;
    
        string query;
         
        getline (cin, query);
        cout << endl;

        engine.ParseUserQuery(query);

        engine.SearchQuery();

        engine.booleanLogic();

        engine.DisplayQueryResult();

        //engine.Clear();

        cout << "Do you want to continue? (enter -1 for exiting this search engine):";
        cout << endl;

        cin >> choice;
        cout << endl;

        if (choice == -1)
        {
            break;
        }
       
        cin.ignore(); //without clearing this (input buffer), getline will not take input after cin >> choice
    }

    return 0;
}