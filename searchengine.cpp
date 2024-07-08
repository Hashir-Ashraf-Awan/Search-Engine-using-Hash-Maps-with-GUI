#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <map>
#include <set>
#include <sstream>
#include <dirent.h>
#include <algorithm>


using namespace std;

class SearchEngine
{
private:
    unordered_map <string, unordered_map <string, int> > ParsedWord;
    vector <string> QueryWords;
    vector <string> boolWords;
    vector <unordered_map <string, int> > QueryResults;
    unordered_map < string, int> relevanceScores;
    map <string, set <string> > fileConnectivity;
    unordered_map <string, vector <unordered_map <string, int> > > queryCache;

public:
    SearchEngine(){}

    void Clear(){ParsedWord.clear();QueryWords.clear();boolWords.clear();QueryResults.clear();relevanceScores.clear();fileConnectivity.clear();}
    void ParseFile(const string &filePath)
    {
        ifstream file(filePath);

        if (file.is_open())
        {
            string line;
            unordered_map<string, int> wordCount;

            while (getline(file, line))
            {
                istringstream iss(line);
                string word;

                while (iss >> word)
                {
                    transform(word.begin(), word.end(), word.begin(), ::tolower);

                    if (wordCount.find(word) == wordCount.end())
                    {
                        wordCount[word] = 1;
                    }
                    else
                    {
                        wordCount[word]++;
                    }
                }

                getFileNames(filePath);
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

        if (directory == NULL)
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

            }
        }

        closedir(directory);
    }


    void ParseUserQuery(const string &query)
    {
        istringstream userQuery(query);
        string queryWord;

        if (query.empty())
        {

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
                    transform(queryWord.begin(), queryWord.end(), queryWord.begin(), ::tolower);
                    QueryWords.push_back(queryWord);
                }
            }
        }
    }


    void SearchQuery(const string &query)
    {
        relevanceScores.clear();
        QueryResults.clear();

        string queryKey = query;

        if (queryCache.find(queryKey) != queryCache.end())
        {
            QueryResults = queryCache[queryKey];
            return;
        }

        for (int i = 0; i < QueryWords.size(); ++i)
        {
            unordered_map<string, int> storeWords;
            unordered_map <string, unordered_map <string, int> > :: iterator it;
            it = ParsedWord.begin();

            for (it = ParsedWord.begin(); it != ParsedWord.end(); ++it)
            {
                const string &filePath = it->first;
                const unordered_map<string, int> &wordCountMap = it->second;

                if (wordCountMap.find(QueryWords[i]) != wordCountMap.end())
                {
                    storeWords[filePath] = wordCountMap.at(QueryWords[i]);
                    relevanceScores[filePath] += wordCountMap.at(QueryWords[i]);
                }
            }

            QueryResults.push_back(storeWords);
        }

        queryCache[queryKey] = QueryResults;
    }

    void booleanLogic()
    {
        if (boolWords.empty())
        {
            return;
        }
        else
        {
            vector <string> :: iterator it2;
            it2 = boolWords.begin();

            for (it2 = boolWords.begin(); it2 != boolWords.end(); ++it2)
            {
                if (*it2 == "AND" || *it2==" ")
                {
                    AND();
                }
                else if (*it2 == "OR")
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

        unordered_map<string, int> map1 = QueryResults[0];
        unordered_map<string, int> mapResult;

        for (int i = 1; i < QueryResults.size(); ++i)
        {
            for (const auto& pair : QueryResults[i])
            {
                if (map1.find(pair.first) != map1.end())
                {
                    mapResult[pair.first] = map1[pair.first] + pair.second;
                }
            }
            map1 = mapResult;
            mapResult.clear();
        }

        QueryResults.clear();
        QueryResults.push_back(map1);
    }


    void OR()
    {
        if (QueryResults.size() < 2)
        {
            return;
        }

        unordered_map<string, int> mapResult = QueryResults[0];

        for (int i = 1; i < QueryResults.size(); ++i)
        {
            for (const auto& pair : QueryResults[i])
            {
                mapResult[pair.first] += pair.second;
            }
        }

        QueryResults.clear();
        QueryResults.push_back(mapResult);
    }


    void DisplayQueryResult()
    {
        unordered_map <string, int> cumulativeScores;
        vector < unordered_map <string, int> > :: iterator it;
        it = QueryResults.begin();

        for (it = QueryResults.begin(); it != QueryResults.end(); ++it)
        {
            unordered_map <string, int> :: iterator it2;
            it2 = it->begin();

            for (it2 = it->begin(); it2 != it->end(); ++it2)
            {
                cumulativeScores[it2->first] += it2->second;
            }
        }

        vector <pair <string, int> > sortedDocuments(cumulativeScores.begin(), cumulativeScores.end());

        sort(sortedDocuments.begin(), sortedDocuments.end(), [&](const pair<string, int> &a, const pair<string, int> &b) {
            return a.second > b.second;
        });

        if (!sortedDocuments.empty())
        {
            vector <pair <string, int> > :: iterator it3;
            it3 = sortedDocuments.begin();

            for (it3 = sortedDocuments.begin(); it3 != sortedDocuments.end(); ++it3)
            {
                cout << "The document is: " << it3->first << " with relevance score: " << it3->second << endl;
            }
        }
        else
        {
            cout << "No document found" << endl;
        }
    }

    vector<unordered_map <string, int> > final()
    {
        return QueryResults;
    }

    void getFileNames(const string &filePath)
    {
        ifstream file(filePath);

        if (file.is_open())
        {
            string line;

            while (getline(file, line))
            {
                istringstream iss(line);
                string word;

                while (iss >> word)
                {
                    if (word.length() >= 4 && word.substr(word.length() - 4) == ".txt")
                    {
                        fileConnectivity[filePath].insert(word);
                    }
                }
            }
        }
        else
        {
            cout << "Error: File not found" << endl;
        }
    }


};
