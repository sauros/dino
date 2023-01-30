#include "package.hpp"
#include <sauros/capi/capi.hpp>
#include <string>

#include <git2.h>

#include <stdio.h>
#include <unistd.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

namespace {
bool loaded_libgit{false};
int (*libgit_clone_)(git_repository **, const char *, const char *,
                     const git_clone_options *){nullptr};
int (*libgit_init_)(){nullptr};
int (*libgit_shutdown_)(){nullptr};
void (*libgit_free_repo_)(git_repository *){nullptr};
const git_error *(*libgit_get_last_error_)(){nullptr};
void *handle_libgit{nullptr};
} // namespace

#define LOAD_SYMBOLS                                                           \
  handle_libgit = dlopen("libgit2.so", RTLD_LAZY);                             \
  if (!handle_libgit) {                                                        \
    fprintf(stderr, "%s\n", dlerror());                                        \
    return std::make_shared<sauros::cell_c>(sauros::cell_type_e::INTEGER,      \
                                            (sauros::cell_int_t)-1);           \
  }                                                                            \
  dlerror();                                                                   \
  libgit_clone_ =                                                              \
      (int (*)(git_repository **, const char *, const char *,                  \
               const git_clone_options *))dlsym(handle_libgit, "git_clone");   \
  libgit_init_ = (int (*)())dlsym(handle_libgit, "git_libgit2_init");          \
  libgit_shutdown_ = (int (*)())dlsym(handle_libgit, "git_libgit2_shutdown");  \
  libgit_free_repo_ =                                                          \
      (void (*)(git_repository *))dlsym(handle_libgit, "git_repository_free"); \
  libgit_get_last_error_ =                                                     \
      (const git_error *(*)())dlsym(handle_libgit, "git_error_last");          \
  char *__dl_error = dlerror();                                                \
  if (__dl_error) {                                                            \
    fprintf(stderr, "%s\n", __dl_error);                                       \
    dlclose(handle_libgit);                                                    \
    return std::make_shared<sauros::cell_c>(sauros::cell_type_e::INTEGER,      \
                                            (sauros::cell_int_t)-1);           \
  }

#define UNLOAD_SYMBOLS                                                         \
  dlclose(handle_libgit);                                                      \
  handle_libgit = nullptr;                                                     \
  loaded_libgit = false;

sauros::cell_ptr _pkg_clone_(sauros::cells_t &cells,
                             std::shared_ptr<sauros::environment_c> env) {

  auto url_data = c_api_process_cell(cells[1], env)->data_as_str();
  auto path_data = c_api_process_cell(cells[2], env)->data_as_str();

  LOAD_SYMBOLS

  libgit_init_();

  git_repository *repo = nullptr;
  int error =
      libgit_clone_(&repo, url_data.c_str(), path_data.c_str(), nullptr);

  if (error != 0) {
    const git_error *err = libgit_get_last_error_();
    if (err)
      printf("ERROR %d: %s\n", err->klass, err->message);
    else
      printf("ERROR %d: no detailed info\n", error);
  } else if (repo) {
    libgit_free_repo_(repo);
  }

  libgit_shutdown_();

  UNLOAD_SYMBOLS

  return std::make_shared<sauros::cell_c>(sauros::cell_type_e::INTEGER,
                                          (sauros::cell_int_t)1);
}
