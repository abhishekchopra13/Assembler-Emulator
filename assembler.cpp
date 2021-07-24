#include <bits/stdc++.h>
#include <stdlib.h> 
using namespace std;

vector<string> Global_errors; // Storing all the errors in during the compiling part
vector<string> mnemonics; // mnemonics enocoded
vector<string> Global_warnings; // Storing all the warnings during the compiling part
vector<string> Listing_File;
vector<string> Object_File;
set<string> used_labels;
map<int, int> Instruction_Line;
map<string, int> Optab;

void init()
{
    mnemonics = {"ldc","adc","ldl","stl","ldnl","stnl","add","sub","shl","shr","adj","a2sp","sp2a","call","return","brz","brlz","br","HALT","data"};
}

int get_opcode_value (string& current_opcode) {
    for (int i = 0; i < (int)mnemonics.size(); i++) {
        if (mnemonics[i] == current_opcode) {
            return i;
        }
    }
    return -1;
}

string num_to_string(int num)
{
    string num_str;
    while (num) {
        num_str.push_back('0' + num%10);
        num /= 10;
    }
    reverse(num_str.begin(), num_str.end());
    return num_str;
}

bool has_char (string &str) 
{
    for (char ch : str) {
        if (ch >= 'a' and ch <= 'z') 
            return true;
        if (ch >= 'A' and ch <= 'Z')
            return true;
    }
    return false;
}

string to_hex (int decimal_value, int len)
{
    std::stringstream ss;
    ss<< std::hex << decimal_value; // int decimal_value
    std::string res (ss.str());
    // Append 0's at back to make size = 4;
    reverse(res.begin(), res.end());
    while ((int)res.size() < len) {
        res.push_back('0');
    }
    while ((int)res.size() > len) {
        res.pop_back();
    }
    reverse(res.begin(), res.end());
    return res;
}

pair<long int, bool> read_operand (string &operand)
{
    if ((int)operand.size() == 0) {
        return {0, true};
    }
    int len = (int)operand.size();
    char *str = (char *)malloc(len *  sizeof(char));
    for (int i = 0; i < (int)operand.size(); i++) {
        str[i] = operand[i];
    }
    for (int i = (int)operand.size(); i < strlen(str); i++) {
        str[i] = '\0';
    }
    char *end;
    long int num;
    num = strtol(str, &end, 10);
    if (!*end) {
        return {num, true};
    }
    num = strtol(str, &end, 8);
    if (!*end) {
        return {num, true};
    }
    num = strtol(str, &end, 16);
    if (!*end) {
        return {num, true};
    }
    return {-1, false};
}

bool correct_label(string &str)
{
    if ((int)str.size() == 0) {
        return false;
    }
    if (str[0] >= 'A' and str[0] <= 'Z') {
        return true;
    }
    if (str[0] >= 'a' and str[0] <= 'z') {
        return true;
    }
    return false;
}

map <string, int> mapLabels (vector<string> &Read_Lines)
{
    map<string, int> label_adrr;
    int count_intr = 0;
    for (int i = 0; i < (int)Read_Lines.size(); i++) {
        // Counting the number of ":" int cnt_label_triggers
        int cnt_label_triggers = 0;
        for (char ch : Read_Lines[i]) {
            cnt_label_triggers += (ch == ':');
        }
        if (cnt_label_triggers == 0) {
            count_intr++;
            continue;
        }
        if (cnt_label_triggers > 1) {
            string error = "Incorrect label format at line ";
            error += num_to_string(Instruction_Line[i]);
            Global_errors.push_back(error);
            continue;
        }
        string label;
        for (char ch : Read_Lines[i]) {
            if (ch == ':')
                break;
            label.push_back(ch);
        }
        if (!correct_label(label)) {
            string error = "Incorrect label naming format at line ";
            error += num_to_string(Instruction_Line[i]);
            Global_errors.push_back(error);
            continue;   
        }
        if (get_opcode_value(label) != -1) {
            string error = "Label name \"" + label + "\" is a keyword at line ";
            error += num_to_string(Instruction_Line[i]);
            Global_errors.push_back(error);
            continue;
        }
        if (label_adrr.find(label) != label_adrr.end()) {
            string error = "Repeated label at line ";
            error += num_to_string(Instruction_Line[i]);
            Global_errors.push_back(error);
            continue;
        }
        label_adrr[label] = count_intr;
        stringstream ss(Read_Lines[i]);
        string s1, s2, s3;
        ss >> s1;
        ss >> s2;
        ss >> s3;
        if ((int)s2.size() > 0) {
            count_intr++;
        }
        if (s2 == "SET") {
            label_adrr[label] = read_operand(s3).first;
        }
    }
    return label_adrr;
}

void remove_comments_spaces(vector<string> &Read_Lines)
{

    vector<string> modified_lines;
    for (int i = 0; i < (int)Read_Lines.size(); i++) {
        string before_comment;
        bool initial_white_space = true;
        for (char ch : Read_Lines[i]) {
            if (ch == ';') 
                break;
            if (ch != ' ' and ch != '\t') {
                initial_white_space = false;
            }
            if (initial_white_space && ch == ' ' or ch == '\t') {
                continue;
            }
            before_comment.push_back(ch);
        }
        if (has_char(before_comment)) {
            Instruction_Line[(int)modified_lines.size()] = i + 1;
            modified_lines.push_back(before_comment);
            // cout << before_comment << endl;
        }
    }
    Read_Lines = modified_lines;
}



void Optable_initialisation(){
    Optab["data"] = 1 ;
    Optab["ldc"] = 1;
    Optab["adc"] = 1;
    Optab["ldl"] = 1;
    Optab["stl"] = 1;
    Optab["ldnl"] = 1;
    Optab["stnl"] = 1 ;
    Optab["add"] = 0;
    Optab["sub"] = 0;
    Optab["shl"] = 0;
    Optab["shr"] = 0;
    Optab["adj"] = 1 ;
    Optab["a2sp"] = 0 ;
    Optab["sp2a"] = 0 ;
    Optab["call"] = 2 ;
    Optab["return"] = 0 ;
    Optab["brz"] = 2 ;
    Optab["brlz"] = 2 ;
    Optab["br"] = 2 ;
    Optab["halt"] = 0 ;
    Optab["set"] = 1 ;
}



int main(int argc, char* argv[])
{
    freopen(argv[1], "r", stdin);
    init(); // Initialising mnemonics
    vector<string> Read_Lines;
    string s;
    while (getline(cin, s))
    {
        Read_Lines.push_back(s);
    }
    // Clearing all the data by removing comments and whitespaces from the lines
    remove_comments_spaces(Read_Lines);
    map<string, int> label_adrr = mapLabels(Read_Lines);
    // for (auto it : label_adrr) {
    //     cout << it.first << ' ' << it.second << '\n';
    // }
    // Pass 1 completed
    
    Optable_initialisation();
    int count_intr = 0;
    for (int i = 0; i < (int)Read_Lines.size(); i++) {
        stringstream ss(Read_Lines[i]);
        string current_opcode;
        ss >> current_opcode;
        if (current_opcode.back() != ':') {
            int opcodeVal = get_opcode_value(current_opcode);
            if (opcodeVal == -1) {
                string error = "Bogus mnemonic at line ";
                error += num_to_string(Instruction_Line[i]);
                Global_errors.push_back(error);
                continue;
            }
            stringstream chk(Read_Lines[i]);
            string str1; int cnt = 0;
            while (chk >> str1) {
                //cerr << str << endl;
                cnt++;
            }
            if (cnt > 2) {
                string error = "More operands than expected at line " + num_to_string(Instruction_Line[i]);
                Global_errors.push_back(error);
                continue;
            }
            if (Optab[current_opcode] == 2) {
                string operand; ss >> operand;
                used_labels.insert(operand);
                if (label_adrr.find(operand) == label_adrr.end()) {
                    string error = operand + " not found as label at line " + num_to_string(Instruction_Line[i]);
                    Global_errors.push_back(error);
                    continue;
                }
                string code = to_hex(count_intr, 4) + " " + to_hex(label_adrr[operand] - count_intr - 1, 6) + to_hex(opcodeVal, 2);
                if (current_opcode == "call") {
                    code = to_hex(count_intr, 4) + " " + to_hex(label_adrr[operand], 6) + to_hex(opcodeVal, 2);
                }
                Listing_File.push_back(code + " " + Read_Lines[i]);
                Object_File.push_back(code);
                count_intr++;
                continue;
            }
            if (current_opcode == "data") {
                string operand; ss >> operand;
                if ((int)operand.size() == 0) {
                    string error = "Operand missing at line " + num_to_string(Instruction_Line[i]);
                    Global_errors.push_back(error);
                    continue;
                }
                pair<long int, bool> val = read_operand(operand);
                if (!val.second) {
                    string error = "Invalid value format at line ";
                    error += num_to_string(Instruction_Line[i]);
                    Global_errors.push_back(error);
                    continue;
                }
                if (val.first > INT_MAX or val.first < INT_MIN) {
                    string warn = "Integer overflow at line " + num_to_string(Instruction_Line[i]);
                    Global_warnings.push_back(warn);
                }
                string code = to_hex(count_intr++, 4) + " " + to_hex(val.first, 8);
                Listing_File.push_back(code + " " + Read_Lines[i]);
                Object_File.push_back(code);
                continue;
            }
            string operand;
            ss >> operand;
            used_labels.insert(operand);
            if (Optab[current_opcode] && (int)operand.size() == 0) {
                string error = "Operand missing at line " + num_to_string(Instruction_Line[i]);
                Global_errors.push_back(error);
                continue;
            } else if (!Optab[current_opcode] && (int)operand.size()) {
                string error = "Extra Operand at line " + num_to_string(Instruction_Line[i]);
                Global_errors.push_back(error);
                continue;
            }
            if ((int)operand.size() > 0 and operand[0] >= 'a' and operand[0] <= 'z') {
                if (label_adrr.find(operand) == label_adrr.end()) {
                    string error = operand + " not found as label at line " + num_to_string(Instruction_Line[i]);
                    Global_errors.push_back(error);
                    continue;
                }
                string code = to_hex(count_intr, 4) + " " + to_hex(label_adrr[operand] , 6) + to_hex(opcodeVal, 2);
                Listing_File.push_back(code + " " + Read_Lines[i]);
                Object_File.push_back(code);
                count_intr++;
                continue;
            } else if ((int)operand.size() > 0 and operand[0] >= 'A' and operand[0] <= 'Z') {
                if (label_adrr.find(operand) == label_adrr.end()) {
                    string error = operand + " not found as label at line " + num_to_string(Instruction_Line[i]);
                    Global_errors.push_back(error);
                    continue;
                }
                string code = to_hex(count_intr, 4) + " " + to_hex(label_adrr[operand] , 6) + to_hex(opcodeVal, 2);
                Listing_File.push_back(code + " " + Read_Lines[i]);
                Object_File.push_back(code);
                count_intr++;
                continue;
            }
            pair<long int, bool> val = read_operand(operand);
            if (!val.second) {
                string error = "Invalid operand format at line ";
                error += num_to_string(Instruction_Line[i]);
                Global_errors.push_back(error);
                continue;
            }
            string code = to_hex(count_intr++, 4) + " " + to_hex(val.first, 6) + to_hex(opcodeVal, 2);
            Listing_File.push_back(code + " " + Read_Lines[i]);
            Object_File.push_back(code);
        } else {
            string next_opcode;
            ss >> next_opcode;
            string prev_opcode = current_opcode;
            current_opcode = next_opcode;
            stringstream chk(Read_Lines[i]);
            string str1; int cnt = 0;
            while (chk >> str1) {
                //cerr << str << endl;
                cnt++;
            }
            if (cnt > 3) {
                string error = "More operands than expected at line " + num_to_string(Instruction_Line[i]);
                Global_errors.push_back(error);
                continue;
            }
            if ((int)current_opcode.size() == 0) {
                Listing_File.push_back(to_hex(count_intr, 4) + " " + Read_Lines[i]);
                continue;
            }
            if (current_opcode == "SET") {
                string operand; 
                ss >> operand;
                pair<long int, bool> val = read_operand(operand);
                if (!val.second or (int)operand.size()==0) {
                    string error = "Missing operand at line ";
                    error += num_to_string(Instruction_Line[i]);
                    Global_errors.push_back(error);
                    continue;
                }
                string code = to_hex(count_intr++, 4) + " " + to_hex(val.first, 8);
                Listing_File.push_back(code + " " + Read_Lines[i]);
                Object_File.push_back(code);
                current_opcode.pop_back();
                prev_opcode.pop_back();
                // label_adrr[prev_opcode] = val.first;
                continue;
            }
            if (current_opcode == "data") {
                string operand; ss >> operand;
                if ((int)operand.size() == 0) {
                    string error = "Operand missing at line " + num_to_string(Instruction_Line[i]);
                    Global_errors.push_back(error);
                    continue;
                }
                pair<long int, bool> val = read_operand(operand);
                if (!val.second) {
                    string error = "Invalid value format at line ";
                    error += num_to_string(Instruction_Line[i]);
                    Global_errors.push_back(error);
                    continue;
                } 
                if (val.first > INT_MAX or val.first < INT_MIN) {
                    string warn = "Integer overflow at line " + num_to_string(Instruction_Line[i]);
                    Global_warnings.push_back(warn);
                }
                string code = to_hex(count_intr++, 4) + " " + to_hex(val.first, 8);
                Listing_File.push_back(code + " " + Read_Lines[i]);
                Object_File.push_back(code);
                continue;
            }
            int opcodeVal = get_opcode_value(current_opcode);
            if (opcodeVal == -1) {
                string error = "Bogus mnemonic at line ";
                error += num_to_string(Instruction_Line[i]);
                Global_errors.push_back(error);
                continue;
            }
            if (Optab[current_opcode] == 2) {
                string operand; ss >> operand;
                used_labels.insert(operand);
                if (label_adrr.find(operand) == label_adrr.end()) {
                    string error = operand + " not found as label at line " + num_to_string(Instruction_Line[i]);
                    Global_errors.push_back(error);
                    continue;
                }
                string code = to_hex(count_intr, 4) + " " + to_hex(label_adrr[operand] - count_intr - 1, 6) + to_hex(opcodeVal, 2);
                if (current_opcode == "call") {
                    code = to_hex(count_intr, 4) + " " + to_hex(label_adrr[operand], 6) + to_hex(opcodeVal, 2);
                }
                Listing_File.push_back(code + " " + Read_Lines[i]);
                Object_File.push_back(code);
                count_intr++;
                continue;
            }
            string operand;
            ss >> operand;
            used_labels.insert(operand);
            if (Optab[current_opcode] && (int)operand.size() == 0) {
                string error = "Operand missing at line " + num_to_string(Instruction_Line[i]);
                Global_errors.push_back(error);
                continue;
            } else if (!Optab[current_opcode] && (int)operand.size()) {
                string error = "Extra Operand at line " + num_to_string(Instruction_Line[i]);
                Global_errors.push_back(error);
                continue;
            }
            if ((int)operand.size() > 0 and operand[0] >= 'a' and operand[0] <= 'z') {
                if (label_adrr.find(operand) == label_adrr.end()) {
                    string error = operand + " not found as label at line " + num_to_string(Instruction_Line[i]);
                    Global_errors.push_back(error);
                    continue;
                }
                string code = to_hex(count_intr, 4) + " " + to_hex(label_adrr[operand], 6) + to_hex(opcodeVal, 2);
                Listing_File.push_back(code + " " + Read_Lines[i]);
                Object_File.push_back(code);
                count_intr++;
                continue;
            } else if ((int)operand.size() > 0 and operand[0] >= 'A' and operand[0] <= 'Z') {
                if (label_adrr.find(operand) == label_adrr.end()) {
                    string error = operand + " not found as label at line " + num_to_string(Instruction_Line[i]);
                    Global_errors.push_back(error);
                    continue;
                }
                string code = to_hex(count_intr, 4) + " " + to_hex(label_adrr[operand], 6) + to_hex(opcodeVal, 2);
                Listing_File.push_back(code + " " + Read_Lines[i]);
                Object_File.push_back(code);
                count_intr++;
                continue;
            }
            pair<long int, bool> val = read_operand(operand);
            if (!val.second) {
                string error = "Invalid operand format at line ";
                error += num_to_string(Instruction_Line[i]);
                Global_errors.push_back(error);
                continue;
            }
            string code = to_hex(count_intr++, 4) + " " + to_hex(val.first, 6) + to_hex(opcodeVal, 2);
            Listing_File.push_back(code + " " + Read_Lines[i]);
            Object_File.push_back(code);
        }
    }
    string file_name;
    for (int i = 0; i < strlen(argv[1]); i++) {
        if (argv[1][i] == '.')
            break;
        else
            file_name.push_back(argv[1][i]);
    }
    if ((int)Global_errors.size() > 0) {
        //system("Color E4");
        file_name += ".log";
        cout << "Compilation error check " << file_name << endl;
        ofstream log_file;
        log_file.open(file_name);
        for (auto itr : label_adrr) {
            if (!used_labels.count(itr.first)) {
                Global_errors.push_back("Warning: unused label \"" + itr.first + "\"");
            }
        }
        for (auto itr : Global_warnings) {
            Global_errors.push_back("Warning: " + itr);
        }
        for (auto itr : Global_errors) {
            log_file << itr << '\n';
        }
        log_file.close();
        return 0;
    } else {
        for (auto itr : label_adrr) {
            if (!used_labels.count(itr.first)) {
                Global_errors.push_back("Warning: unused label \"" + itr.first + "\"");
            }
        }
        for (auto itr : Global_warnings) {
            Global_errors.push_back("Warning: " + itr);
        }
        // system("Color E4");
        ofstream log_file;
        log_file.open(file_name + ".log");
        for (auto itr : Global_errors) {
            log_file << itr << '\n';
        }
        log_file.close();
    }
    // system("Color DE");
    cout << "Succesfully Compiled check " << file_name + ".l"<< endl;
    ofstream list_file;
    list_file.open(file_name+".l");
    for (auto itr : Listing_File) {
        list_file << itr << '\n';
    }
    list_file.close();
    ofstream machine_code;
    file_name += ".o";
    machine_code.open(file_name, ios::binary | ios::out);
    for (auto it : Object_File) {
        stringstream V(it);
        string s1, s2;
        V >> s1 >> s2;
        int num = 0;
        reverse(s2.begin(), s2.end());
        for (int i = 0; i < 8; i++) {
            int val = 0;
            if (s2[i] >= 'a' and s2[i] <= 'f') {
                val = (s2[i]-'a'+10);
            } else {
                val = s2[i] - '0';
            }
            num += (val * (int)pow(16, i));
        }
        static_cast<int>(num);
        machine_code.write((const char*)&num, sizeof(int));
    }
    machine_code.close();
    return 0;
}