/**
 *
 * @file interrupts.cpp
 * @author Marwa Diab
 *
 */

#include "interrupts.hpp"

int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/
    int current_time = 0;          
    const int ctx = 10;            
    const int isr_time = 40; 

    /******************************************************************/
    //parse each line of the input trace file
     while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);
         
        /******************ADD YOUR SIMULATION CODE HERE*************************/
         
        if (activity == "CPU") {
            execution += std::to_string(current_time) + ", " +
                         std::to_string(duration_intr) + ", CPU Burst\n";
            current_time += duration_intr;
        }
        else if (activity == "SYSCALL") {
            int device = duration_intr;
            if (device < 0 || device >= (int)delays.size() || device >= (int)vectors.size()) {
                std::cerr << "Invalid device: " << device << "\n";
                continue;
            }

            // trap to kernel + context save + vector lookup + load PC
            auto result = intr_boilerplate(current_time, device, ctx, vectors);
            execution += result.first;
            current_time = result.second;

            // execute the system call (software handler)
            execution += std::to_string(current_time) + ", " +
                         std::to_string(isr_time) + ", execute system call for device " +
                         std::to_string(device) + "\n";
            current_time += isr_time;

            // start the device I/O 
            execution += std::to_string(current_time) + ", 1, start I/O on device " +
                         std::to_string(device) + "\n";
            current_time += 1;

            // return to user 
            execution += std::to_string(current_time) + ", " +
                         std::to_string(ctx) + ", return to user (context restored)\n";
            current_time += ctx;
        }
        else if (activity == "END_IO") {
            int device = duration_intr;
            if (device < 0 || device >= (int)delays.size() || device >= (int)vectors.size()) {
                std::cerr << "Invalid device: " << device << "\n";
                continue;
            }

            // device completion interrupt
            auto result = intr_boilerplate(current_time, device, ctx, vectors);
            execution += result.first;
            current_time = result.second;

            // service completion
            execution += std::to_string(current_time) + ", " +
                         std::to_string(isr_time) + ", service device " +
                         std::to_string(device) + " completion\n";
            current_time += isr_time;

            // return from interrupt
            execution += std::to_string(current_time) + ", " +
                         std::to_string(ctx) + ", return from interrupt (context restored)\n";
            current_time += ctx;
        }

        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
