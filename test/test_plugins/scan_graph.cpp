﻿/* Copyright (c) 2022 AntGroup. All Rights Reserved. */

#include <iostream>
#include "lgraph/lgraph.h"

#include "tools/json.hpp"

using json = nlohmann::json;

using namespace lgraph_api;

extern "C" LGAPI bool Process(GraphDB &db, const std::string &request, std::string &response) {
    bool scan_edges = false;
    int times = 10;
    try {
        std::cout << "  input: " << request << std::endl;
        json input = json::parse(request);
        scan_edges = input["scan_edges"].get<bool>();
        times = input["times"].get<int>();
    } catch (std::exception &e) {
        response = std::string("error parsing json: ") + e.what();
        return false;
    }
    auto txn = db.CreateReadTxn();
    size_t num_vertices = 0;
    size_t num_edges = 0;
    for (int i = 0; i < times; i++)
        for (auto vit = txn.GetVertexIterator(); vit.IsValid(); vit.Next()) {
            num_vertices += 1;
            if (scan_edges) {
                for (auto eit = vit.GetOutEdgeIterator(); eit.IsValid(); eit.Next()) {
                    num_edges += 1;
                }
            }
        }
    json output;
    output["num_vertices"] = num_vertices;
    if (scan_edges) {
        output["num_edges"] = num_edges;
    }
    response = output.dump();
    return true;
}
