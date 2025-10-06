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
    int current_time = 0;                
    const int ctx = 10;                   
    const int isr_time = 40;              
    auto line = [&](int start, int dur, const std::string& msg) {
        return std::to_string(start) + ", " + std::to_string(dur) + ", " + msg + "\n";
    };

    /******************************************************************/

    //parse each line of the input trace file
     while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);
         
        /******************ADD YOUR SIMULATION CODE HERE*************************/
         
        if (activity == "CPU") {
            execution += line(current_time, duration_intr, "CPU Burst");
            current_time += duration_intr;
        }
        else if (activity == "SYSCALL") {
            int device = duration_intr;

            if (device < 0 || device >= (int)delays.size() || device >= (int)vectors.size()) {
                std::cerr << "Invalid device: " << device << "\n";
                continue;
            }

            // trap to kernel + context save + vector lookup + load PC
            auto [prelog, new_time] = intr_boilerplate(current_time, device, ctx, vectors);
            execution += prelog;
            current_time = new_time;

            // run the system-call handler (software path)
            execution += line(current_time, isr_time, "execute system call for device " + std::to_string(device));
            current_time += isr_time;

            // start the device I/O (device runs asynchronously)
            execution += line(current_time, 1, "start I/O on device " + std::to_string(device));
            current_time += 1;

            // return to user (restore context)
            execution += line(current_time, ctx, "return to user (context restored)");
            current_time += ctx;

            // NOTE: we do NOT wait for the device delay here.
            
        }
        else if (activity == "END_IO") {
            int device = duration_intr;

            if (device < 0 || device >= (int)delays.size() || device >= (int)vectors.size()) {
                std::cerr << "Invalid device: " << device << "\n";
                continue;
            }

            // device asserts an interrupt,trap to kernel and handle completion
            auto [prelog, new_time] = intr_boilerplate(current_time, device, ctx, vectors);
            execution += prelog;
            current_time = new_time;

            // finish I/O: service completion
            execution += line(current_time, isr_time, "service device " + std::to_string(device) + " completion");
            current_time += isr_time;

            // return from interrupt to user mode
            execution += line(current_time, ctx, "return from interrupt (context restored)");
            current_time += ctx;
        }

        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
