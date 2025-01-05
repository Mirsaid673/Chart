#include "Chart.h"

int main() {
    std::unique_ptr<AppBase> app = std::make_unique<Chart>();
    return app->run();
}