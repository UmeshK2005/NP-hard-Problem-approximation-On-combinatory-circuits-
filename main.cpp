#include<cstdlib>
#include <iostream>
#include<fstream>
#include<unordered_map>
#include<map>
#include<vector>
#include<climits>
#include <string>
#include <algorithm>
#include <cmath>
#include <ctime>
using namespace std;

class gate_type{
public:
    double area;
    double delay;

    gate_type(double delay,double area){
        this->area = area;
        this->delay = delay;
    }
};

class gate_cluster{
public:
    int counter; 
    gate_type* first;
    gate_type* second;
    gate_type* third;

    gate_cluster(){
        this->counter =0;
        this->first = NULL;
        this->second = NULL;
        this->third = NULL;
    }
};

class Node{
public:
    string type;
    double delay_val;
    vector<Node*> inputs;
    vector<Node*> outputs;

    Node(double value){
        delay_val = value;
    }
};

bool mycomp(pair<double,double> p1,pair<double,double> p2){
    return p1.first < p2.first;
}

string to_base3(int n,int b,int nodes){
    string ans;
    if(n==0){
        ans = ans+'0';
    }else{
        while(n > 0){
            ans = to_string(n%b)+ans;
            n = n/b; 
        }
    }
    while(ans.size()<nodes){
        ans = '0'+ans;
    }
    return ans;
}

double actual_delay(Node* &output_node,unordered_map<Node*,double> &answer,unordered_map<string,double>&gate_num,unordered_map<string,gate_cluster*>&delays,unordered_map<Node*,string> str){
    if((output_node->inputs).size()==0){
        answer[output_node] =0;
        return 0;
    }if((output_node->inputs).size()!=0){
        //std::std::cout<<str[output_node]<<" -> "<<gate_num[str[output_node]]<<endl;
        if(answer.find(output_node)!= answer.end()){
            return answer[output_node];
        }else{
            double ad;
            
            ad = actual_delay(output_node->inputs[0], answer,gate_num,delays,str);
            for(int i=0;i<(output_node->inputs).size();i++){
                ad = max(ad,actual_delay(output_node->inputs[i] ,answer,gate_num,delays,str));
            }
            double delay_value;
            
            if(gate_num[str[output_node]]==0){
                delay_value = delays[output_node->type]->first->delay;
            }else if(gate_num[str[output_node]]==1){
                delay_value = delays[output_node->type]->second->delay;
            }else{
                delay_value = delays[output_node->type]->third->delay;
            }
            //std::std::cout<<str[output_node]<<" "<<delay_value<<endl;
            ad = ad+delay_value;
            answer[output_node]=ad;
            return ad;
        }
    }return 0;
}

double time_avail(double &temp,unordered_map<Node*,double> &th,Node* &output_node,unordered_map<Node*,double> &answer,unordered_map<string,gate_cluster*>&delays,unordered_map<Node*,string> str){
    //std::cout<<"Time avail"<<endl;
    if((output_node->inputs).size()==0){
        answer[output_node]=0;
        return 0;
    }else{
        if(answer.find(output_node) != answer.end()){
            return answer[output_node];
        }else{
            double ad;
            ad = time_avail(temp,th,output_node->inputs[0],answer,delays,str);
            for(int i=0;i<(output_node->inputs.size());i++){
                ad = max(ad,time_avail(temp,th,output_node->inputs[i],answer,delays,str));
            }
            double small = delays[output_node->type]->third->delay;
            double medium = delays[output_node->type]->second->delay;
            double large = delays[output_node->type]->first->delay;
            double th_value = th[output_node];
            if(ad+small <= th_value){
                ad = ad+small;
                temp = temp+ delays[output_node->type]->third->area;
                //cout<<str[output_node]<<" -> "<<delays[output_node->type]->third->area<<endl;
            }else if(ad+small > th_value && ad+medium <= th_value){
                ad = ad+medium;
                temp = temp+ delays[output_node->type]->second->area;
                //cout<<str[output_node]<<" -> "<<delays[output_node->type]->second->area<<endl;
            }else{
                ad = ad+large;
                temp = temp+ delays[output_node->type]->first->area;
                //cout<<str[output_node]<<" -> "<<delays[output_node->type]->first->area<<endl;
            }
            answer[output_node] = ad;
            return ad;
        }
    }
}

double required_delay(double delay_constraint,Node* &input_node,unordered_map<Node*,double> &answer2,unordered_map<Node*,string> str,unordered_map<string,int> s,unordered_map<string,double> gate_num,unordered_map<string,gate_cluster*> delays){
    //std::cout<<"Required Delay"<<endl;
    if((input_node->outputs).size()==0){
        if(s[str[input_node]]==1){
            answer2[input_node]=INT_MAX;
            return answer2[input_node];
        }
        answer2[input_node]= delay_constraint;
        return answer2[input_node];
    }
    if((input_node->outputs).size()!=0){
        if(answer2.find(input_node)!=answer2.end()){
            return answer2[input_node];
        }else{
            double rd;
            double sub_value;
            if(gate_num[str[input_node->outputs[0]]]==0){
                sub_value = delays[input_node->outputs[0]->type]->first->delay;
            }else if(gate_num[str[input_node->outputs[0]]]==1){
                sub_value = delays[input_node->outputs[0]->type]->second->delay;
            }else{
                sub_value = delays[input_node->outputs[0]->type]->third->delay;
            }
            rd = required_delay(delay_constraint,input_node->outputs[0],answer2, str,s,gate_num,delays) - sub_value;
            for(int i=0;i<(input_node->outputs).size();i++){
                if(gate_num[str[input_node->outputs[i]]]==0){
                    sub_value = delays[input_node->outputs[i]->type]->first->delay;
                }else if(gate_num[str[input_node->outputs[i]]]==1){
                    sub_value = delays[input_node->outputs[i]->type]->second->delay;
                }else{
                    sub_value = delays[input_node->outputs[i]->type]->third->delay;
                }
                    rd = min(rd,required_delay(delay_constraint,input_node->outputs[i],answer2, str , s,gate_num,delays)-sub_value);
            }
            answer2[input_node]=rd;
            return rd;
        }
    }return 0;
}

double random_min(int nodes, double delay_constraint,vector<string>&delay_calculate,unordered_map<string,Node*>&node,unordered_map<Node*,string>&str,unordered_map<string,gate_cluster*>&delays,vector<string> gates){
    double min_area = INT_MAX;
    int loop_limit = (100000.0/nodes);
    for(int ix=0;ix<loop_limit;ix++){
        
        srand(ix+1);
        double temp =0;
        unordered_map<string,double> gate_num;
        for(auto i:gates){
            gate_num[i] = abs(rand())%3;
            if(gate_num[i]==0){
                temp = temp+ delays[node[i]->type]->first->area;
            }else if(gate_num[i]==1){
                temp = temp+ delays[node[i]->type]->second->area;
            }else{
                temp = temp+ delays[node[i]->type]->third->area;
            }
        }
        unordered_map<Node*,double> answer2;
        for(auto i:delay_calculate){
            actual_delay(node[i],answer2, gate_num ,delays,str);
        }
        double ans=INT_MIN;
            
        for(auto i:delay_calculate){
            ans = max(ans,answer2[node[i]]); 
        }

        if(ans <= delay_constraint){
            min_area = min(min_area,temp);
        }
    }
    return min_area;
}

double optimised_area(int nodes,double delay_constraint,unordered_map<double,string>&check,vector<string>&delay_calculate,unordered_map<string,Node*>&node,unordered_map<Node*,string>&str,unordered_map<string,gate_cluster*>&delays,vector<string> gates,unordered_map<string,int> primary_inputs,unordered_map<string,int> s){
    if(nodes <=10){
        
        double min_area = INT_MAX;
        int loop_limit = pow(3,nodes);
        for(int i=0; i<loop_limit;i++){
            string s = to_base3(i,3,nodes);
            unordered_map<string,double> gate_num;
            double temp =0;
            int j=0;
            unordered_map<Node*,double> answer2;
            for(auto i:gates){////Ask whether traversal is always in same manner.
                gate_num[i] = s[j]-'0';
                if(s[j]-'0' == 0){
                    //std::std::cout<<i<<" "<<delays[node[i]->type]->first->area<<endl;
                    temp = temp+ delays[node[i]->type]->first->area;
                }else if(s[j]-'0'==1){
                    //std::std::cout<<i<<" "<<delays[node[i]->type]->second->area<<endl;
                    temp = temp+ delays[node[i]->type]->second->area;
                }else{
                    //std::std::cout<<i<<" "<<delays[node[i]->type]->third->area<<endl;
                    temp = temp+ delays[node[i]->type]->third->area;
                }
                
                j++;
            }
            // cout<<"--------------------"<<endl;
            // cout<<s<<endl;
            // for(auto i: gate_num){
            //     std::cout<<i.first<<" -> "<<i.second<<endl;
            // }cout<<"-----------------------"<<endl;
            
            for(auto i:delay_calculate){
                actual_delay(node[i],answer2,gate_num,delays,str);
            }
            double ans=INT_MIN;
            
            for(auto i:delay_calculate){
                ans = max(ans,answer2[node[i]]); 
            }
            if(ans <= delay_constraint){
                check[temp] = s; 
                min_area = min(min_area,temp);
            }
        }
        return min_area;
    }else{
        double ans4;
        
        ans4 = random_min(nodes,delay_constraint,delay_calculate,node,str,delays,gates);
        
        unordered_map<Node*,double> answer2;
        unordered_map<Node*,double> answer;
        unordered_map<string,double> gate_num;
        for(auto i:gates){
            gate_num[i] = 0;
        }
        for(auto i:primary_inputs){
            required_delay(delay_constraint,node[i.first],answer2,str,s,gate_num,delays);
        }
        double ans2=0;
        for(auto i:delay_calculate){
            time_avail(ans2,answer2, node[i],answer,delays,str);
        }
        double ans1=INT_MAX;
        
        for(int ix=0;ix<min(100000/nodes,10000);ix++){
        srand(ix+1);
        unordered_map<Node*,double> answer21;
        unordered_map<Node*,double> answer1;
        for(auto i:gates){
            gate_num[i] = abs(rand())%3;
        }
        for(auto i:primary_inputs){
            required_delay(delay_constraint,node[i.first],answer21,str,s,gate_num,delays);
        }double ans3=0;
        for(auto i:delay_calculate){
            time_avail(ans3,answer21,node[i],answer1,delays,str);
        }
        ans1 = min(ans1,ans3);
    }
        
        
        double a = min(ans1,ans2);
        return min(a,ans4);
    }

    return 0;
}

int main(int args,char* arg[]){
    fstream circuit_file;
    fstream gate_delay_file;
    fstream longest_comb_delay_file;
    fstream delay_constr_file;
    fstream min_area_file;
    unordered_map<string,gate_cluster*> delays;
    unordered_map<string,Node*> node;
    unordered_map<Node*,string> str;
    unordered_map<Node*,double> answer;
    unordered_map<string,int> internal_sig;
    unordered_map<string,int> primary_outputs;
    unordered_map<string,int> primary_inputs;
    vector<string> delay_calculate;
    vector<string> gates;
    int nodes =0;
    circuit_file.open(arg[2],ios::in);
    gate_delay_file.open(arg[3],ios::in);
    delay_constr_file.open(arg[4],ios::in);
    if(gate_delay_file.is_open()){
        string line;
        while(getline(gate_delay_file,line)){
            
            if(line[0] == '/'){
                ;
            }else if(line.size()==0){
                ;
            }else{
                string s;
                int i=0;
                while(line[i] !=' ' && i<line.size()){
                    i++;
                }i++;
                while(line[i]!=' ' && i<line.size()){
                    s = s+line[i];
                    i++;
                }i++;
                string delay;
                while(line[i]!=' '&& i< line.size()){
                    delay = delay+line[i];
                    i++;
                }i++;
                string area;
                while(line[i]!=' ' && i<line.size()){
                    area = area+line[i];
                    i++;
                }
                
                if(delays.find(s) ==delays.end()){
                    gate_cluster* g = new gate_cluster();
                
                    g->first = new gate_type(stod(delay),stod(area));
                    
                    g->counter++;
                    delays[s]=g;
                    
                }else{
                    if(delays[s]->counter ==1){
                        delays[s]->second = new gate_type(stod(delay),stod(area));
                        delays[s]->counter++;
                    }else{
                        delays[s]->third = new gate_type(stod(delay),stod(area));
                        delays[s]->counter++;
                    }
                }
            }
        }
        gate_delay_file.close();
    }
    
    for(auto i: delays){
        vector<pair<double,double>> v;
        v.push_back(make_pair(i.second->first->delay,i.second->first->area));
        v.push_back(make_pair(i.second->second->delay,i.second->second->area));
        v.push_back(make_pair(i.second->third->delay,i.second->third->area));
        sort(v.begin(),v.end(),mycomp);
        i.second->first->delay = v[0].first;
        i.second->first->area = v[0].second;
        i.second->second->delay = v[1].first;
        i.second->second->area = v[1].second;
        i.second->third->delay = v[2].first;
        i.second->third->area = v[2].second;
    }

    if(circuit_file.is_open()){
        string line;
        while(getline(circuit_file,line)){
            if(line[0]=='/'){
                ;
            }else if(line.size() ==0){
                ;
            }else{
                string s;
                int i=0;
                while(line[i]!=' ' && i<line.size()){
                    s = s+line[i];
                    i++;
                }
                if(s=="PRIMARY_INPUTS" || s=="PRIMARY_OUTPUTS" || s=="INTERNAL_SIGNALS"){
                    
                    string s1;
                    i++;
                    while(i<line.size()){
                        if(line[i]==' '){
                            Node* n = new Node(0);
                            node[s1] = n;
                            str[n] = s1;
                            if(s=="PRIMARY_OUTPUTS"){
                                delay_calculate.push_back(s1);
                                primary_outputs[s1] =1;
                            }if(s=="PRIMARY_INPUTS"){
                                primary_inputs[s1] =1;
                            }if(s=="INTERNAL_SIGNALS"){
                                internal_sig[s1] =1;
                            }
                            s1 ="";
                        }else{
                            s1 = s1+line[i];
                        }i++;
                    }Node* n = new Node(0);
                    node[s1]=n;
                    str[n]=s1;
                    if(s=="PRIMARY_OUTPUTS"){
                        delay_calculate.push_back(s1);
                        primary_outputs[s1] =1;
                    }if(s=="PRIMARY_INPUTS"){
                        primary_inputs[s1] =1;
                    }if(s=="INTERNAL_SIGNALS"){
                        internal_sig[s1] = 1;
                    }
                }else{
                    
                    if(s=="DFF"){
                        
                        string input;
                        i++;
                        while(line[i]!=' ' && i<line.size()){
                            input = input+line[i];
                            i++;
                        }i++;
                        string output;
                        while(line[i]!=' ' && i<line.size()){
                            output = output+line[i];
                            i++;
                        }delay_calculate.push_back(input);
                        primary_outputs[input] =1;
                        primary_inputs[output]=1;
                        node[output]->type = "DFF";
                    }else{
                        
                        nodes = nodes+1;
                        int j=line.size()-1;
                        while(line[j]==' '){
                            j--;
                        }string output;
                        while(line[j]!=' '){
                            output = line[j]+output;
                            j--;
                        }node[output]->delay_val = delays[s]->first->delay;/// assigning delay_val of output_node as fastest
                        node[output]->type = s;
                        gates.push_back(output);
                        string input;
                        j--;
                        while(j>=i){
                            if(line[j]==' '){
                                (node[input]->outputs).push_back(node[output]);
                                (node[output]->inputs).push_back(node[input]);
                                input ="";
                            }else{
                                input = line[j]+input;
                            }j--;
                        }
                    }
                }
            }
        }
        circuit_file.close();
    }
    
    string A = arg[1];
    double delay_constraint;
    if(delay_constr_file.is_open()){
        string line;
        while(getline(delay_constr_file,line)){
            delay_constraint = stoi(line);
        }
    }

    for(auto it: internal_sig){
        if(primary_outputs[it.first]==1){
            internal_sig[it.first] =0;
        }
        if(primary_inputs[it.first] ==1){
            internal_sig[it.first] =0;
        }
    }
    ///////////////// Part-A//////////////////////
    
    unordered_map<double,string> check;
  ///////////// Part-B ///////////////////////////
    if(A == "A"){
        unordered_map<string,double> gate_num;
        for(auto i:gates){
            gate_num[i] =0;
        }
        for(auto i:delay_calculate){
            actual_delay(node[i],answer,gate_num,delays,str);
        }
        double ans=INT_MIN;
        for(auto i:delay_calculate){
            ans = max(ans,answer[node[i]]); 
        }
        longest_comb_delay_file.open("longest_delay.txt",ios::out);
        if(longest_comb_delay_file.is_open()){
            longest_comb_delay_file<<ans<<endl;
        }
        longest_comb_delay_file.close();
    }
    else{
        int start = time(0);
        unordered_map<string,double> gate_num;
        for(auto i:gates){
            gate_num[i] =0;
        }
        double ansb = optimised_area(nodes,delay_constraint,check,delay_calculate,node,str,delays,gates,primary_inputs,internal_sig);
        min_area_file.open("minimum_area.txt",ios::out);
        if(min_area_file.is_open()){
            min_area_file<<ansb<<endl;
        }
        min_area_file.close();
        cout<<"Time Taken : "<<time(0)-start<<" sec"<<endl;
    }
}
