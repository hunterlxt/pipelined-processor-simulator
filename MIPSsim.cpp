/* On my honor, I have neither given nor received unauthorized aid on this assignment */

/*
    liu xintao
    51174500112
    12/24/2017
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <bitset>
using namespace std;
struct instruction //用来存放原始指令的相关信息
{
    int id = -1;
    int rs;
    int rt;
    int rd;
    int sa;
    int addr;
    int imme;
    int p;
    bitset<32> instr;
    string format;
};
vector<instruction> instr_map;
struct mem_data
{
    bitset<32> instr;
    int data;
};
vector<mem_data> mem;
int data_pos;
int reg[32];
bitset<32> temp_bit;
vector<string> instr_name = {"J", "JR", "BEQ", "BLTZ", "BGTZ", "BREAK", "SW", "LW", "SLL", "SRL", "SRA", "NOP",
                             "ADD", "SUB", "MUL", "AND", "OR", "XOR", "NOR", "SLT", "ADDI", "ANDI", "ORI", "XORI"};

//simulator用到的
int max_queue = 0;
struct instruction previous;
int PC = 256;
vector<int> reg_branch_ready;
vector<int> pre_reg_branch_ready;
vector<int> reg_state; //1代表ALU1在用，2代表ALU2在用,0没有在用
vector<int> pre_reg_state;
bool pre_issue_stalled = false;
bool flag_break = true;
struct Cycle
{
    vector<instruction> waiting_instr;
    vector<instruction> executing;
    vector<instruction> pre_issue_queue;
    vector<instruction> pre_alu1;
    vector<instruction> pre_alu2;
    vector<instruction> pre_mem;
    vector<instruction> post_alu2;
    vector<instruction> post_mem;
} prev_cycle, cycle;
vector<instruction> executed_instr;
int max_pre_issue = 0;
ofstream sim_file("simulation.txt");
//以上simulator用到的
int number = 1;
//以上是print用到的
void read_file(string s);
void disassembler_print();
void simulator();

int main(int argc, char *argv[])
{
    string filename = argv[1];
    read_file(filename);
    disassembler_print();
    simulator();
    return 0;
}

/********************************************************************************************/

void read_file(string s)
{
    ifstream inputfile(s);
    string line;
    bool flag = true;
    while (getline(inputfile, line))
    {
        if (flag)
        {
            struct instruction temp_instr;
            string str;
            str = line.substr(6, 5);
            bitset<5> temp5(str);
            temp_instr.rs = temp5.to_ulong();
            str = line.substr(11, 5);
            bitset<5> temp5_1(str);
            temp_instr.rt = temp5_1.to_ulong();
            str = line.substr(16, 5);
            bitset<5> temp5_2(str);
            temp_instr.rd = temp5_2.to_ulong();
            str = line.substr(21, 5);
            bitset<5> temp5_3(str);
            temp_instr.sa = temp5_3.to_ulong();
            str = line.substr(6, 26);
            bitset<26> temp26(str);
            temp_instr.addr = temp26.to_ulong();
            str = line.substr(16, 16);
            bitset<16> temp16(str);
            temp_instr.imme = temp16.to_ulong();
            bitset<32> temp32(line);
            temp_instr.instr = temp32;
            if (line.compare(0, 2, "01") == 0) //第一类指令的判断
            {
                if (line.compare(2, 4, "0000") == 0)
                {
                    temp_instr.id = 0;
                    temp_instr.addr *= 4;
                }
                else if (line.compare(2, 4, "0001") == 0)
                {
                    temp_instr.id = 1;
                }
                else if (line.compare(2, 4, "0010") == 0)
                {
                    temp_instr.id = 2;
                    temp_instr.imme *= 4;
                }
                else if (line.compare(2, 4, "0011") == 0)
                {
                    temp_instr.id = 3;
                    temp_instr.imme *= 4;
                }
                else if (line.compare(2, 4, "0100") == 0)
                {
                    temp_instr.id = 4;
                    temp_instr.imme *= 4;
                }
                else if (line.compare(2, 4, "0101") == 0)
                {
                    temp_instr.id = 5;
                    flag = false;
                }
                else if (line.compare(2, 4, "0110") == 0)
                {
                    temp_instr.id = 6;
                }
                else if (line.compare(2, 4, "0111") == 0)
                {
                    temp_instr.id = 7;
                }
                else if (line.compare(2, 4, "1000") == 0)
                {
                    temp_instr.id = 8;
                }
                else if (line.compare(2, 4, "1001") == 0)
                {
                    temp_instr.id = 9;
                }
                else if (line.compare(2, 4, "1010") == 0)
                {
                    temp_instr.id = 10;
                }
                else if (line.compare(2, 4, "1011") == 0)
                {
                    temp_instr.id = 11;
                }
            }
            else if (line.compare(0, 2, "11") == 0) //第二类指令的判断
            {
                if (line.compare(2, 4, "0000") == 0)
                {
                    temp_instr.id = 12;
                }
                else if (line.compare(2, 4, "0001") == 0)
                {
                    temp_instr.id = 13;
                }
                else if (line.compare(2, 4, "0010") == 0)
                {
                    temp_instr.id = 14;
                }
                else if (line.compare(2, 4, "0011") == 0)
                {
                    temp_instr.id = 15;
                }
                else if (line.compare(2, 4, "0100") == 0)
                {
                    temp_instr.id = 16;
                }
                else if (line.compare(2, 4, "0101") == 0)
                {
                    temp_instr.id = 17;
                }
                else if (line.compare(2, 4, "0110") == 0)
                {
                    temp_instr.id = 18;
                }
                else if (line.compare(2, 4, "0111") == 0)
                {
                    temp_instr.id = 19;
                }
                else if (line.compare(2, 4, "1000") == 0)
                {
                    temp_instr.id = 20;
                }
                else if (line.compare(2, 4, "1001") == 0)
                {
                    temp_instr.id = 21;
                }
                else if (line.compare(2, 4, "1010") == 0)
                {
                    temp_instr.id = 22;
                }
                else if (line.compare(2, 4, "1011") == 0)
                {
                    temp_instr.id = 23;
                }
            }
            instr_map.push_back(temp_instr);
        }
        else
        {
            struct mem_data temp_mem;
            bitset<32> bit(line);
            temp_mem.instr = bit;
            if (bit.test(31))
            {
                bit.flip();
                temp_mem.data = -(bit.to_ulong() + 1);
            }
            else
                temp_mem.data = bit.to_ulong();
            mem.push_back(temp_mem);
        }
    }
    inputfile.close();
}

void disassembler_print()
{
    ofstream dis_file("disassembly.txt");
    int PC = 256;

    for (int i = 0; i < instr_map.size(); i++)
    {
        instr_map[i].p = i;
        dis_file << instr_map[i].instr.to_string() << '\t' << PC << '\t';
        instr_map[i].format = instr_name[instr_map[i].id];
        switch (instr_map[i].id)
        {
        case 0:
            instr_map[i].format += " #" + to_string(instr_map[i].addr);
            break;
        case 1:
            instr_map[i].format += " R" + to_string(instr_map[i].rs);
            break;
        case 2:
            instr_map[i].format += " R" + to_string(instr_map[i].rs) + ',' + ' ' + 'R' + to_string(instr_map[i].rt) + ',' + ' ' + '#' + to_string(instr_map[i].imme);
            break;
        case 3:
        case 4:
            instr_map[i].format += " R" + to_string(instr_map[i].rs) + ',' + ' ' + '#' + to_string(instr_map[i].imme);
            break;
        case 5:
            data_pos = PC + 4;
            break;
        case 6:
        case 7:
            instr_map[i].format += " R" + to_string(instr_map[i].rt) + ',' + ' ' + to_string(instr_map[i].imme) + '(' + 'R' + to_string(instr_map[i].rs) + ')';
            break;
        case 8:
        case 9:
        case 10:
            instr_map[i].format += " R" + to_string(instr_map[i].rd) + ',' + ' ' + 'R' + to_string(instr_map[i].rt) + ',' + ' ' + '#' + to_string(instr_map[i].sa);
            break;
        case 11:
            break;
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
            instr_map[i].format += " R" + to_string(instr_map[i].rd) + ',' + ' ' + 'R' + to_string(instr_map[i].rs) + ',' + ' ' + 'R' + to_string(instr_map[i].rt);
            break;
        case 20:
        case 21:
        case 22:
        case 23:
            instr_map[i].format += " R" + to_string(instr_map[i].rt) + ',' + ' ' + 'R' + to_string(instr_map[i].rs) + ',' + ' ' + '#' + to_string(instr_map[i].imme);
            break;
        }
        dis_file << instr_map[i].format << endl;
        PC += 4;
    }
    for (auto each : mem)
    {
        dis_file << each.instr.to_string() << '\t' << PC << '\t' << each.data << endl;
        PC += 4;
    }
}

bool try_execute_branch() //如果执行了，更新IF分支队列，返回true，否则只返回false
{
    struct instruction waiting_instr = cycle.waiting_instr.front();
    switch (waiting_instr.id)
    {
    case 0:
        PC = waiting_instr.addr;
        executed_instr.push_back(waiting_instr);
        cycle.waiting_instr.pop_back(); //-1表示这里没指令了
        pre_issue_stalled = false;
        return true;
        break;
    case 1:
        if (pre_reg_branch_ready[waiting_instr.rs] == 0)
        {
            PC = reg[waiting_instr.rs];
            executed_instr.push_back(waiting_instr);
            cycle.waiting_instr.pop_back();
            pre_issue_stalled = false;
            return true;
        }
        break;

    case 2:
        if (pre_reg_branch_ready[waiting_instr.rs] == 0 && pre_reg_branch_ready[waiting_instr.rt] == 0)
        {
            if (reg[waiting_instr.rs] == reg[waiting_instr.rt])
                PC += (4 + waiting_instr.imme);
            else
                PC += 4;
            executed_instr.push_back(waiting_instr);
            cycle.waiting_instr.pop_back();
            pre_issue_stalled = false;
            return true;
        }
        break;
    case 3:
        if (pre_reg_branch_ready[waiting_instr.rs] == 0)
        {
            if (reg[waiting_instr.rs] < 0)
                PC += (4 + waiting_instr.imme);
            else
                PC += 4;
            executed_instr.push_back(waiting_instr);
            cycle.waiting_instr.pop_back();
            pre_issue_stalled = false;
            return true;
        }
        break;
    case 4:
        if (pre_reg_branch_ready[waiting_instr.rs] == 0)
        {
            if (reg[waiting_instr.rs] > 0)
                PC += (4 + waiting_instr.imme);
            else
                PC += 4;
            executed_instr.push_back(waiting_instr);
            cycle.waiting_instr.pop_back();
            pre_issue_stalled = false;
            return true;
        }
        break;
    default:
        break;
    }
    return false;
}

void new_reg_branch(struct instruction instr)
{
    switch (instr.id)
    {
    case 7:
        reg_branch_ready[instr.rt] = 1;
        break;
    case 8:
        reg_branch_ready[instr.rd] = 1;
        break;
    case 9:
        reg_branch_ready[instr.rd] = 1;
        break;
    case 10:
        reg_branch_ready[instr.rd] = 1;
        break;
    case 12:
        reg_branch_ready[instr.rd] = 1;
        break;
    case 13:
        reg_branch_ready[instr.rd] = 1;
        break;
    case 14:
        reg_branch_ready[instr.rd] = 1;
        break;
    case 15:
        reg_branch_ready[instr.rd] = 1;
        break;
    case 16:
        reg_branch_ready[instr.rd] = 1;
        break;
    case 17:
        reg_branch_ready[instr.rd] = 1;
        break;
    case 18:
        reg_branch_ready[instr.rd] = 1;
        break;
    case 19:
        reg_branch_ready[instr.rd] = 1;
        break;
    case 20:
        reg_branch_ready[instr.rt] = 1;
        break;
    case 21:
        reg_branch_ready[instr.rt] = 1;
        break;
    case 22:
        reg_branch_ready[instr.rt] = 1;
        break;
    case 23:
        reg_branch_ready[instr.rt] = 1;
        break;
    default:
        break;
    }
}

void IF()
{
    int pos = (PC - 256) / 4;

    if (pre_issue_stalled == false && prev_cycle.pre_issue_queue.size() < 4)
    {
        if (instr_map[pos].id == 5) //先判断是不是break指令
        {
            flag_break = false;
            executed_instr.push_back(instr_map[pos]);
            return;
        }
        if (instr_map[pos].id != 0 && instr_map[pos].id != 1 && instr_map[pos].id != 2 && instr_map[pos].id != 3 && instr_map[pos].id != 4)
        {
            if (cycle.pre_issue_queue.size() < 4)
            {
                if (instr_map[pos].id == 11) //如果是NOP空指令，单独处理
                {
                    max_pre_issue++;
                    PC += 4;
                    if (max_pre_issue == max_queue)
                        return;
                    IF();
                }

                //不是NOP也不是分支的往pre-issue里塞
                cycle.pre_issue_queue.push_back(instr_map[pos]);
                new_reg_branch(instr_map[pos]);
                PC += 4;
                max_pre_issue++;
                if (max_pre_issue == max_queue)
                    return;
                IF();
                return;
            }
        }
        else
        {
            if (max_pre_issue == max_queue)
                return;
            cycle.waiting_instr.push_back(instr_map[pos]);
            if (try_execute_branch() == false)
                pre_issue_stalled = true;
            return;
        }
    }
    // if (cycle.pre_issue_queue.size() == 0 && cycle.pre_alu1.size() == 0 && cycle.pre_alu2.size() == 0 && cycle.pre_mem.size() == 0 && cycle.post_mem.size() == 0 && cycle.post_alu2.size() == 0)
    // bool flag_wb = true;
    // for (auto ele : all_wb)
    // {
    //     if (ele == 1)
    //         flag_wb = false;
    // }
    // if (flag_wb)
    // {
    if (cycle.waiting_instr.size() != 0)
        try_execute_branch();
    if (cycle.executing.size() != 0)
    {
        executed_instr.push_back(cycle.executing.front());
        cycle.executing.pop_back();
    }
    return;
}

bool hazard(int index) //index表示要判断的那个指令的下标,true表示有冒险
{
    instruction instr = prev_cycle.pre_issue_queue[index];
    int rs = instr.rs;
    int rt = instr.rt;
    int rd = instr.rd;
    bool flag = false;
    for (int i = 0; i < index; i++)
    {
        previous = prev_cycle.pre_issue_queue[i];
        int pre_w[32] = {0};
        int pre_r[32] = {0};
        int r[32] = {0};
        int w[32] = {0};
        switch (instr.id)
        {
        case 6:
            r[rs] = 1;
            r[rt] = 1;
            break;
        case 8:
        case 9:
        case 10:
            r[rt] = 1;
            w[rd] = 1;
            break;
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
            r[rs] = 1;
            r[rt] = 1;
            w[rd] = 1;
            break;
        case 7:
        case 20:
        case 21:
        case 22:
        case 23:
            r[rs] = 1;
            w[rt] = 1;
            break;
        }
        switch (previous.id)
        {
        case 6:
            pre_r[previous.rs] = 1;
            pre_r[previous.rt] = 1;
            break;
        case 8:
        case 9:
        case 10:
            pre_r[previous.rt] = 1;
            pre_w[previous.rd] = 1;
            break;
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
            pre_r[previous.rs] = 1;
            pre_r[previous.rt] = 1;
            pre_w[previous.rd] = 1;
            break;
        case 7:
        case 20:
        case 21:
        case 22:
        case 23:
            pre_r[previous.rs] = 1;
            pre_w[previous.rt] = 1;
            break;
        }
        for (int x = 0; x < 32; x++)
        {
            if (pre_w[x] == 1 && w[x] == 1)
                flag = true;
        }
        for (int x = 0; x < 32; x++)
        {
            if (pre_w[x] == 1 && r[x] == 1)
                flag = true;
        }
        for (int x = 0; x < 32; x++)
        {
            if (pre_r[x] == 1 && w[x] == 1)
                flag = true;
        }
    }
    return flag;
}

void Issue()
{
    bool issued = false;
    for (int k = 0, i = 0; k < prev_cycle.pre_issue_queue.size(); k++, i++) //k代表pre里的位置，i代表现在的位置
    {
        if (issued)
            i--;
        issued = false;
        auto instr = cycle.pre_issue_queue[i];
        if (instr.id == 6 || instr.id == 7 && cycle.pre_alu1.size() == 0)
        {
            if (instr.id == 6 && !hazard(k))
            {
                bool not_issue = false;
                //判断前面有没有还没issue的store
                for (int j = 0; j < i; j++)
                {
                    if (cycle.pre_issue_queue[j].id == 6)
                        not_issue = true;
                }
                if (pre_reg_state[instr.rs] == 0 && pre_reg_state[instr.rt] == 0 && not_issue == false)
                {
                    cycle.pre_alu1.push_back(instr);
                    cycle.pre_issue_queue.erase(cycle.pre_issue_queue.begin() + i);
                    issued = true;
                }
            }
            else if (!hazard(k))
            {
                bool not_issue = false;
                //判断前面有没有还没issue的store
                for (int j = 0; j < i; j++)
                {
                    if (cycle.pre_issue_queue[j].id == 6)
                        not_issue = true;
                }
                if (not_issue == false)
                {
                    if (pre_reg_state[instr.rs] == 0)
                    {
                        cycle.pre_alu1.push_back(instr);
                        reg_state[instr.rt] = 1;
                        cycle.pre_issue_queue.erase(cycle.pre_issue_queue.begin() + i);
                        issued = true;
                    }
                }
            }
        }
        else if (!hazard(k) && cycle.pre_alu2.size() == 0)
        {
            switch (instr.id)
            {
            case 8:
            case 9:
            case 10:
                if (pre_reg_state[instr.rt] == 0)
                {
                    cycle.pre_alu2.push_back(instr);
                    reg_state[instr.rd] = 2;
                    //删除第i个pre-issue里的东西
                    cycle.pre_issue_queue.erase(cycle.pre_issue_queue.begin() + i);
                    issued = true;
                }
                break;
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
                if (pre_reg_state[instr.rs] == 0 && pre_reg_state[instr.rt] == 0)
                {
                    cycle.pre_alu2.push_back(instr);
                    reg_state[instr.rd] = 2;
                    //删除第i个pre-issue里的东西
                    cycle.pre_issue_queue.erase(cycle.pre_issue_queue.begin() + i);
                    issued = true;
                }
                break;
            case 20:
            case 21:
            case 22:
            case 23:
                if (pre_reg_state[instr.rs] == 0)
                {
                    cycle.pre_alu2.push_back(instr);
                    reg_state[instr.rt] = 2;
                    //删除第i个pre-issue里的东西
                    cycle.pre_issue_queue.erase(cycle.pre_issue_queue.begin() + i);
                    issued = true;
                }
                break;
            default:
                break;
            }
        }
    }

    return;
}

void ALU1()
{
    if (cycle.pre_alu1.size() != 0)
    {
        cycle.pre_mem.push_back(cycle.pre_alu1.front());
        cycle.pre_alu1.erase(cycle.pre_alu1.begin());
        return;
    }
}

void ALU2()
{
    if (cycle.pre_alu2.size() != 0)
    {
        cycle.post_alu2.push_back(cycle.pre_alu2.front());
        cycle.pre_alu2.erase(cycle.pre_alu2.begin());
        return;
    }
}

void Mem()
{
    if (cycle.pre_mem.size() == 1)
    {
        struct instruction instr = cycle.pre_mem.front();
        cycle.pre_mem.pop_back();
        switch (instr.id)
        {
        case 6:
            mem[((reg[instr.rs] + instr.imme) - data_pos) / 4].data = reg[instr.rt];
            break;
        case 7:
            cycle.post_mem.push_back(instr);
            break;
        }
    }
    return;
}

void WB()
{
    if (cycle.post_mem.size() != 0)
    {
        struct instruction instr = cycle.post_mem.front();
        cycle.post_mem.pop_back();
        reg[instr.rt] = mem[((reg[instr.rs] + instr.imme) - data_pos) / 4].data;
        reg_state[instr.rt] = 0;
        reg_branch_ready[instr.rt] = 0;
    }
    if (cycle.post_alu2.size() != 0)
    {
        struct instruction instr = cycle.post_alu2.front();
        cycle.post_alu2.pop_back();
        int id = instr.id;
        int rs = instr.rs;
        int rt = instr.rt;
        int rd = instr.rd;
        int sa = instr.sa;
        int imme = instr.imme;
        switch (instr.id)
        {
        case 8:
            temp_bit = reg[rt];
            temp_bit <<= (sa);
            reg[rd] = temp_bit.to_ulong();
            reg_state[rd] = 0;
            reg_branch_ready[rd] = 0;
            break;
        case 9:
            temp_bit = reg[rt];
            temp_bit >>= (sa);
            reg[rd] = temp_bit.to_ulong();
            reg_state[rd] = 0;
            reg_branch_ready[rd] = 0;
            break;
        case 10:
            temp_bit = reg[rt];
            temp_bit >>= (sa);
            reg[rd] = temp_bit.to_ulong();
            reg_state[rd] = 0;
            reg_branch_ready[rd] = 0;
            break;
        case 12:
            reg[rd] = reg[rs] + reg[rt];
            reg_state[rd] = 0;
            reg_branch_ready[rd] = 0;
            break;
        case 13:
            reg[rd] = reg[rs] - reg[rt];
            reg_state[rd] = 0;
            reg_branch_ready[rd] = 0;
            break;
        case 14:
            reg[rd] = reg[rs] * reg[rt];
            reg_state[rd] = 0;
            reg_branch_ready[rd] = 0;
            break;
        case 15:
            reg[rd] = reg[rs] & reg[rt];
            reg_state[rd] = 0;
            reg_branch_ready[rd] = 0;
            break;
        case 16:
            reg[rd] = reg[rs] | reg[rt];
            reg_state[rd] = 0;
            reg_branch_ready[rd] = 0;
            break;
        case 17:
            reg[rd] = reg[rs] ^ reg[rt];
            reg_state[rd] = 0;
            reg_branch_ready[rd] = 0;
            break;
        case 18:
            reg[rd] = ~(reg[rs] ^ reg[rt]);
            reg_state[rd] = 0;
            reg_branch_ready[rd] = 0;
            break;
        case 19:
            if (reg[rs] < reg[rt])
                reg[rd] = 1;
            else
                reg[rd] = 0;
            reg_state[rd] = 0;
            reg_branch_ready[rd] = 0;
            break;
        case 20:
            reg[rt] = reg[rs] + imme;
            reg_state[rt] = 0;
            reg_branch_ready[rt] = 0;
            break;
        case 21:
            reg[rt] = reg[rs] & imme;
            reg_state[rt] = 0;
            reg_branch_ready[rt] = 0;
            break;
        case 22:
            reg[rt] = reg[rs] | imme;
            reg_state[rt] = 0;
            reg_branch_ready[rt] = 0;
            break;
        case 23:
            reg[rt] = reg[rs] ^ imme;
            reg_state[rt] = 0;
            reg_branch_ready[rt] = 0;
            break;
        }
    }

    return;
}

void print_simulator()
{

    for (int i = 20; i > 0; i--)
        sim_file << '-';
    sim_file << endl
             << "Cycle:" << number << endl
             << endl;
    sim_file << "IF Unit:" << endl;
    sim_file << '\t' << "Waiting Instruction:";
    if (cycle.waiting_instr.size() != 0)
    {
        sim_file << " [" << cycle.waiting_instr.front().format << ']';
    }
    sim_file << endl
             << '\t' << "Executed Instruction:";
    if (executed_instr.size() != 0)
    {
        sim_file << " [" << executed_instr.front().format << ']';
        executed_instr.pop_back();
    }
    sim_file << endl
             << "Pre-Issue Queue:";
    int i = 0;
    for (; i < cycle.pre_issue_queue.size(); i++)
    {
        sim_file << endl
                 << '\t' << "Entry " << i << ": [" << cycle.pre_issue_queue[i].format << ']';
    }
    for (; i < 4; i++)
    {
        sim_file << endl
                 << '\t' << "Entry " << i << ':';
    }
    sim_file << endl
             << "Pre-ALU1 Queue:";
    i = 0;
    for (; i < cycle.pre_alu1.size(); i++)
    {
        sim_file << endl
                 << '\t' << "Entry " << i << ": [" << cycle.pre_alu1.front().format << ']';
    }
    for (; i < 2; i++)
    {
        sim_file << endl
                 << '\t' << "Entry " << i << ':';
    }
    sim_file << endl
             << "Pre-MEM Queue:";
    if (cycle.pre_mem.size() != 0)
    {
        sim_file << " [" << cycle.pre_mem.front().format << ']';
    }
    sim_file << endl
             << "Post-MEM Queue:";
    if (cycle.post_mem.size() != 0)
    {
        sim_file << " [" << cycle.post_mem.front().format << ']';
    }
    sim_file << endl
             << "Pre-ALU2 Queue:";
    i = 0;
    for (; i < cycle.pre_alu2.size(); i++)
    {
        sim_file << endl
                 << '\t' << "Entry " << i << ": [" << cycle.pre_alu2.front().format << ']';
    }
    for (; i < 2; i++)
    {
        sim_file << endl
                 << '\t' << "Entry " << i << ':';
    }
    sim_file << endl
             << "Post-ALU2 Queue:";
    if (cycle.post_alu2.size() != 0)
    {
        sim_file << " [" << cycle.post_alu2.front().format << ']';
    }
    sim_file << endl
             << endl
             << "Registers" << endl
             << "R00:";
    for (int i = 0; i < 8; i++)
        sim_file << '\t' << reg[i];
    sim_file << endl;
    sim_file << "R08:";
    for (int i = 8; i < 16; i++)
        sim_file << '\t' << reg[i];
    sim_file << endl;
    sim_file << "R16:";
    for (int i = 16; i < 24; i++)
        sim_file << '\t' << reg[i];
    sim_file << endl;
    sim_file << "R24:";
    for (int i = 24; i < 32; i++)
        sim_file << '\t' << reg[i];
    sim_file << endl
             << endl
             << "Data" << endl;
    int sign = data_pos;
    int num = 0;
    int size = mem.size();
    while (size >= 8)
    {
        sim_file << sign << ':';
        for (int i = 0; i < 8; i++)
        {
            sim_file << '\t' << mem[num + i].data;
        }
        sim_file << endl;
        sign += 32;
        size -= 8;
        num += 8;
    }
    if (size != 0)
    {
        sim_file << sign << ':';
        for (int i = 0; i < size; i++)
        {
            sim_file << '\t' << mem[num + i].data;
        }
        sim_file << endl;
    }
    number++;
    return;
}

void simulator()
{
    for (int i = 0; i < 32; i++)
        pre_reg_state.push_back(0);
    for (int i = 0; i < 32; i++)
        pre_reg_branch_ready.push_back(0);
    while (flag_break)
    {
        cycle = prev_cycle;
        reg_state = pre_reg_state;
        reg_branch_ready = pre_reg_branch_ready;
        max_queue = 4 - prev_cycle.pre_issue_queue.size();
        if (max_queue > 2)
            max_queue = 2;
        WB();
        Mem();
        ALU1();
        ALU2();
        Issue();
        IF();
        max_pre_issue = 0; //必须恢复0,这个用来保证每次最多取2个指令
        pre_reg_branch_ready = reg_branch_ready;
        pre_reg_state = reg_state;
        prev_cycle = cycle;
        print_simulator();
    }
    return;
}