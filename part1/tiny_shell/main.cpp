#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
  while (true) {
    std::string buffer;
    std::cout << ">";
    std::getline(std::cin, buffer);
    boost::trim(buffer);
    if (buffer == "exit") {
       return 0;
    }
    std::vector<std::string> tokens;
    boost::split(tokens, buffer, boost::is_any_of(" \t"));
    auto ret = fork();
    if (ret == -1) {
      std::cerr << "Failed to fork" << std::endl;
      return -1;
    }
    if (ret == 0) {
      if (!tokens.empty()) {
	std::unique_ptr<char*[]> args{new char*[tokens.size() + 1]};
	for (size_t i = 0; i < tokens.size(); ++i) {
	  args[i] = const_cast<char*>(tokens[i].c_str());
	}
	args[tokens.size()] = nullptr;
        execvp(tokens[0].c_str(), args.get());
        return 0;	
      }
    } else {
      waitpid(ret, nullptr, 0);
    }
  }
  return 0;
}
