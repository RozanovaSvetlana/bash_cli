#ifndef BASH_CLI_CAT_HPP
#define BASH_CLI_CAT_HPP

#include "commands_utils.h"

namespace commands {

    class Cat : public Cmd {
    public:

        virtual int run(const job &params, EnvState &env, std::istream &in, std::ostream &out) override {

            if (params.args.empty())
                throw std::invalid_argument("No files were transferred");

            std::stringstream result;
            int32_t error_count = 0;

            for (auto &filename: params.args) {
                fs::path current_path(env.path);
                current_path.replace_filename(filename);

                //проверка на то, существует ли файл в текущей директории
                if (!is_file_exist(current_path)) {
                    if (!is_file_exist(filename)) {
                        ++error_count;
                        result << filename << ": No such file or directory\n";
                        continue;
                    }

                    current_path = filename;
                }

                //проверка на то, можно ли открыть файл на чтение
                if (!is_readable(current_path)) {
                    ++error_count;
                    result << filename << ": Permission denied\n";
                    continue;
                }

                result << get_file_contents(current_path) << "\n";
            }

            if (error_count == params.args.size())
                throw std::invalid_argument(result.str());

            out << result.str();

            if (error_count > 0)
                return 1;

            return 0;
        }

    private:

        static std::string get_file_contents(const fs::path &filename) {
            std::ifstream t(filename);
            std::string result;

            t.seekg(0, std::ios::end);
            result.reserve(t.tellg());
            t.seekg(0, std::ios::beg);

            result.assign((std::istreambuf_iterator<char>(t)),
                          std::istreambuf_iterator<char>());

            return result;
        }

    };
}

#endif //BASH_CLI_CAT_HPP
