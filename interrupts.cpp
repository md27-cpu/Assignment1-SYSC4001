/**
 *
 * @file interrupts.cpp
 * @author Marwa Diab
 *
 */

#include<interrupts.hpp>

int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/
    int currenttime = 0;
    int save_restore_context = 10;
    const int isr = 40;
    std::string log;
    struct Event { std::string activity; int value; };
    std::vector<Event> events;

    /******************************************************************/

    //parse each line of the input trace file
    while (std::getline(input_file, trace)) {
        if (trace.empty()) continue;
        auto [activity, duration_intr] = parse_trace(trace);
        if (activity == "null") continue;

        /******************ADD YOUR SIMULATION CODE HERE*************************/
        // For Part (i): just parse and store input data.
        events.push_back({activity, duration_intr}); 

        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
