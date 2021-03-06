/*
 * Copyright (c) 2008-2014, Chen Huaying <chying.ren@gmail.com>
 * All Rights Reserved.
 */

#include <iostream>
#include <cstdlib>

#include "cluster_slots.h"
using std::cout;
using std::endl;

void RedisNodeGroup::set_master(ClusterRedis *cr)
{
    master_ = cr;
}

void RedisNodeGroup::add_node(ClusterRedis *cr)
{
    nodes_.push_back(cr);
}

void RedisNodeGroup::show_nodes()
{
    if (master_) {
    cout << "master: " << master_->get_ip()
         << ":" << master_->get_port()
         << endl;
    }

    cout << "all nodes: " << endl;
    vector<ClusterRedis *>::iterator itr = nodes_.begin();
    for (; itr != nodes_.end(); ++itr) {
        cout << (*itr)->get_ip() << ":" << (*itr)->get_port() << endl;
    }
}

ClusterRedis *RedisNodeGroup::get_client(CLUSTER_NODE_TYPE type)
{
    if (type == CLUSTER_NODE_MASTER)
        return master_;
    else if (type == CLUSTER_NODE_SLAVE){
        ClusterRedis *cr = NULL;
        if (next_ >= (int32_t)nodes_.size()) {
            next_ = 0;
        }
        cr = nodes_[next_++];
        if (cr == master_) return nodes_[next_++];
        return cr;
    }

    int32_t num = rand_r(&seed_) % nodes_.size();
    return nodes_[num];
}

void RedisNodeGroup::free_clients()
{
    vector<ClusterRedis *>::iterator itr = nodes_.begin();
    for (; itr != nodes_.end(); ++itr) {
        cout << (*itr)->get_ip() << ":" << (*itr)->get_port() << endl;
        (*itr)->UnInit();
        delete *itr;
    }
    nodes_.clear();
}


// Begin of class ClusterSlots
ClusterSlots::ClusterSlots(int32_t from, int32_t to)
    :from_(from), to_(to)
{
}

void ClusterSlots::add_node_info(pair<string, int32_t> ip_port)
{
    ip_ports_.push_back(ip_port);
}

void ClusterSlots::show_slot()
{
    cout << "\t from: " << from_ << "\n"
         << "\t to:   " << to_ << endl;
    vector<pair<string, int32_t> >::iterator itr;
    for (itr = ip_ports_.begin(); itr != ip_ports_.end(); ++itr) {
        cout <<"\t node: " << itr->first << ":" << itr->second << endl;
    }
    node_group_.show_nodes();
}
