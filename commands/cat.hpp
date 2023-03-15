#ifndef BASH_CLI_CAT_HPP
#define BASH_CLI_CAT_HPP

#include "file_utils.hpp"
#include "cmd.hpp"


namespace commands {

/**
 * Implementation of cat command
 */
class Cat : public Cmd {
public:
    /**
     * Concatenates tok.args[0], tok.args[1], ..., tok.args[n] and outputs to the output stream
     * How it is written in interpreter syntax:  <cat file1 file2 ...>
     *
     * Absence of tok.args is valid, however the read is from the incoming stream:
     * cat some text in console
     * some text in console
     *
     * @param params: token with command name in tok.name and command arguments in tok.args
     * @param env: current environment variables of the interpreter
     * @param input: input FILE stream
     * @param output: output FILE stream
     * @param err: error FILE stream (unused)
     * @return 0 if there were no errors, 1 otherwise
     *
     * @throws std::invalid_arguments: Thrown if all files in tok.args not exists or cannot be open
     */
    virtual int run(const token& params, std::shared_ptr<Environment> env, FILE* input, FILE* output, FILE* err) override {
        std::stringstream result, errors;
        int32_t error_count = 0;

        if (params.args.empty()) {
            while (auto line = FileUtils::readLine(input))
                result << line.value();

            FileUtils::writeToFile(result.str(), output);
            return 0;
        }

        for (auto &filename: params.args) {
            std::filesystem::path current_path(env->at("PWD").to_string());
            current_path /= filename;

            // проверка на то, существует ли файл в текущей директории
            if (!FileUtils::is_file_exist(current_path)) {
                if (!FileUtils::is_file_exist(filename)) {
                    ++error_count;
                    errors << params.name << ": " << filename << ": No such file or directory" << std::endl;
                    result << params.name << ": " << filename << ": No such file or directory" << std::endl;

                    continue;
                }
                current_path = filename;
            }

            // проверка на то, можно ли открыть файл на чтение
            if (!FileUtils::is_readable(current_path)) {
                ++error_count;
                errors << filename << ": Permission denied" << std::endl;
                result << filename << ": Permission denied" << std::endl;
                continue;
            }
            result << get_file_contents(current_path);
        }

        if (error_count == params.args.size())
            throw std::invalid_argument(result.str());

        FileUtils::writeToFile(result.str(), output);

        if(!errors.str().empty())
        FileUtils::writeToFile(errors.str(), err);

        if (error_count > 0)
            return 1;
        return 0;
    }

private:

    static std::string get_file_contents(const std::filesystem::path &filename) {
        std::ifstream t(filename);
        std::string result;

        t.seekg(0, std::ios::end);
        result.reserve(t.tellg());
        t.seekg(0, std::ios::beg);

        result.assign((std::istreambuf_iterator<char>(t)),std::istreambuf_iterator<char>());
        return result;
    }
};

} // namespace commands

#endif //BASH_CLI_CAT_HPP
