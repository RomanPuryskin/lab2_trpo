#pragma once
#include <filesystem>
#include <vector>

// Рекурсивный обход папки: возвращает все вложенные файлы
class DirectoryWalker {
    public:
        std::vector<std::filesystem::path> files(const std::filesystem::path& root) const {
            std::vector<std::filesystem::path> result;
            for (const auto& e : std::filesystem::recursive_directory_iterator(root))
                if (e.is_regular_file()) result.push_back(e.path());
            return result;
        }
};
