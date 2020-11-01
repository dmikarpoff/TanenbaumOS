#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>


int main(int argc, char** argv)
{
  while (true) {
    // generic handling of command input
    std::string buffer;
    std::cout << ">";
    std::getline(std::cin, buffer);
    boost::trim(buffer);
    if (buffer == "exit") {
      return 0;
    }
    std::vector<std::string> tokens;
    boost::split(tokens, buffer, boost::is_any_of(" \t"));
    for (auto& t : tokens) {
        boost::trim(t);
    }
    // handle redirection
    std::vector<size_t> redirection_tokens;
    auto is_compare_sign = [] (const auto& str) {
      return str ==  ">" || str == "<" || str == ">>";
    };
    auto it = std::find_if(tokens.begin(), tokens.end(), is_compare_sign);
    while (it != tokens.end()) {
      redirection_tokens.push_back(static_cast<size_t>(it - tokens.begin()));
      it = std::find_if(it + 1, tokens.end(), is_compare_sign);
    }
    redirection_tokens.push_back(tokens.size());

    // fork process from our shell
    auto ret = fork();
    if (ret == -1) {
      std::cerr << "Failed to fork" << std::endl;
      return -1;
    }
    if (ret == 0) {
      if (!tokens.empty()) {
	std::unique_ptr<char*[]> args{new char*[redirection_tokens.front() + 1]};
	for (size_t i = 0; i < redirection_tokens.front(); ++i) {
	  args[i] = const_cast<char*>(tokens[i].c_str());
	}
	args[redirection_tokens.front()] = nullptr;
	for (size_t i = 0; i + 1 < redirection_tokens.size(); ++i) {
	  auto next = redirection_tokens[i + 1];
	  auto cur = redirection_tokens[i];
	  if (next != cur + 2) {
            continue;
	  }
	  const auto& redir_sign = tokens[cur];
	  const auto& redir_file = tokens[cur + 1];
	  int fd = -1;
	  int descr = -1;
	  if (redir_sign == "<") {
            descr = 0;
	    fd = open(redir_file.c_str(), O_RDONLY);
	    if (fd == -1) {
              std::cerr << "Failed to open file " << redir_file << " for reading" << std::endl;
	      return -1;
	    }
	  } else if (redir_sign == ">") {
            descr = 1;
	    fd = open(redir_file.c_str(), O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IROTH);
	    if (fd == -1) {
              std::cerr << "Failed to open file " << redir_file << " for writing" << std::endl;
	      return -1;
	    }
	  } else if (redir_sign == ">>") {
            descr = 1;
	    fd = open(redir_file.c_str(), O_WRONLY | O_APPEND | O_CREAT, S_IRWXU | S_IRWXG | S_IROTH);
	    if (fd == -1) {
              std::cerr << "Failed to open file " << redir_file << " for appending" << std::endl;
	      return -1;
	    }
	  }
	  if (fd == -1) {
            std::cerr << "Redirection failed on " << redir_sign << " " << redir_file 
		      << std::endl;
	    return -1;
	  }
	  if(dup2(fd, descr) == -1) {
            std::cerr << "Failed to duplicate file descriptor" << std::endl;
	    return -1;
	  }
	}
        auto res = execvp(tokens[0].c_str(), args.get());
	if (res == -1) {
	  std::cerr << "Failed to run " << tokens[0] << std::endl;
	  return -1;
	}
      }
    } else {
      waitpid(ret, nullptr, 0);
    }
  }
  return 0;
}
