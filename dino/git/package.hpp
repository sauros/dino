#ifndef SAUROS_PKG_HPP
#define SAUROS_PKG_HPP
#include <sauros/sauros.hpp>
#ifdef WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT
#endif
extern "C" {
/*
   A callable function!
*/
API_EXPORT
extern sauros::cell_ptr _pkg_clone_(sauros::cells_t &cells,
                                    std::shared_ptr<sauros::environment_c> env);
}
#endif
