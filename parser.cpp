#include<iostream>
#include<fstream>
#include<cstdlib>
#include<iomanip>
#include<string>
#include<sstream>
#include<vector>
#include<map>
#include <tuple>
#include<bits/stdc++.h>
#include <typeinfo>
using namespace std;

tuple <map<int, pair<string, int> >, map<int, vector<int> >, map<int, pair<string, string> > > parser(){
    string filename;
    ifstream file;
    string text;
    vector<string> config_detail;  
    //read the config file
    cout<<"Enter name of ConfigFile:";
    getline(cin,filename);
    filename.append(".txt");
    file.open(filename.c_str());
    while(getline(file,text)) {
        if(isdigit(text[0])){
            config_detail.push_back(text);
        }
    }
    file.close();
    map<int, pair<string, int> > port_uid_map;
    map<int, vector<int> > uid_neighbor_map;
    string str;
    int no_nodes = stoi(config_detail[0]);
    for (int i = 0; i < no_nodes; i++){
        // store uid, hostname and port
        int node_uid = 0, port_no = 0, cnt_word = 0;
        string hostName;
        stringstream strstrm(config_detail[i+1]);  
        while (getline(strstrm, str, ' ')){
            if(cnt_word == 0){
                node_uid = stoi(str);
                cnt_word++;
            }
            else if(cnt_word == 1){
                hostName = str;
                cnt_word++;
            }
            else{
                port_no = stoi(str);
                cnt_word++;
            }
        }
        stringstream strstrm2(config_detail[(i+no_nodes)+1]);  
        vector<int> neighbors; 
        while (getline(strstrm2, str, ' ')){
            neighbors.push_back(stoi(str));

        }
        port_uid_map.insert(make_pair(node_uid, make_pair(hostName, port_no))); 
        uid_neighbor_map.insert(make_pair(node_uid, neighbors));    
    }  
    map<int, pair<string, string> > uid_ip_map;
    string ip_format = "10.176.69.";
	int ip_last = 31;
	for (const auto& element : port_uid_map){
        int uid = element.first;
        int fd = stoi(element.second.first.substr(2,1));
        int sd = stoi(element.second.first.substr(3,1));
        int host_no = 10*fd+sd;
        string hname = element.second.first;
        string ip= (ip_format + to_string(host_no+ip_last));
        uid_ip_map.insert(make_pair(uid, make_pair(hname, ip))); 
	}
    return make_tuple(port_uid_map, uid_neighbor_map, uid_ip_map);
}



int main() {
    map<int, pair<string, int> > port_uid_map;
    map<int, vector<int> > uid_neighbor_map;
    map<int, pair<string, string> > uid_ip_map;
    tie(port_uid_map, uid_neighbor_map, uid_ip_map) = parser();
    //Test if the correct contents are being read from the text file.
    for(const auto& element : uid_ip_map){
        cout << element.first << " " << element.second.first << " " << element.second.second << "\n";
    }
    // for(auto itr=uid_neighbor_map.begin(); itr!=uid_neighbor_map.end(); ++itr){
    //     cout << (*itr).first << ": ";
    //     vector <int> inVect = (*itr).second;
    //     for (unsigned j=0; j<inVect.size(); j++){
    //         cout << inVect[j] << " ";
    //     }
    //     cout << endl;
    // }
    return 0;
}


