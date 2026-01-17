# supalib ⚡

**supalib** is a lightweight, embedded-friendly C client library for [Supabase](https://supabase.com).
Designed for IoT devices, embedded systems, and C applications requiring seamless integration with Supabase Authentication (GoTrue) and Database (PostgREST).

## 🚀 Features

- **Authentication (GoTrue)**
  - ✅ User Signup (Registration)
  - ✅ User Login (Email & Password)
  - ✅ Session Management (Access Token handling)
  - ✅ Secure Logout
- **Database (PostgREST)**
  - 🚧 Select Data (GET request) - *Structure ready, implementation in progress*
  - 📝 Insert/Update/Delete - *Planned*
- **Core**
  - 🛡️ Secure header management (API Key & Bearer Token)
  - 💾 Efficient memory management with custom callbacks
  - 📦 **Zero-dependency bloat**: `cJSON` is vendored, only `libcurl` is required externally.

## 🛠️ Dependencies

To build `supalib`, you only need **libcurl** installed on your system.
*Note: `cJSON` is now included directly in the source tree, so you don't need to install it separately.*

### Installation on Windows (MSYS2 / UCRT64)
```bash
pacman -S mingw-w64-ucrt-x86_64-curl
````

### Installation on Linux (Debian/Ubuntu)

Bash

```
sudo apt-get install libcurl4-openssl-dev
```

## ⚙️ Configuration

Before compiling, you must configure your Supabase credentials.

1. Locate the configuration header: `include/private_key.h`
    
2. Update the macros with your project details (found in Supabase Dashboard -> Settings -> API):
    

C

```
// include/private_key.h

#define SUPABASE_URL "[https://your-project-ref.supabase.co](https://your-project-ref.supabase.co)"
#define SUPABASE_KEY "your-anon-public-key"
```

> Security Tip: Do not commit your real keys to a public repository. Use the following command to manage keys locally without tracking them:
> 
> git update-index --skip-worktree include/private_key.h

## 📦 Build & Run

The project includes a cross-platform `Makefile` (Windows & Linux).

Bash

```
# Clean and Build
make clean
make

# Run the CLI Demo
# On Windows:
./app.exe

# On Linux:
./app
```

## 💻 Usage Example

Here is a minimal example of how to initialize the client and perform a login:

C

```
#include <stdio.h>
#include "include/supabase.h"
#include "include/private_key.h"

int main() {
    // 1. Initialize Client
    SupabaseClient* client = supabase_init(SUPABASE_URL, SUPABASE_KEY);
    if (!client) return 1;

    // 2. Perform Login
    if (supabase_login(client, "user@example.com", "my_password") == 0) {
        printf("Login Successful!\n");
        printf("Access Token: %s\n", client->access_token);
        
        // You are now authenticated. Future requests will use the token automatically.
    } else {
        printf("Login Failed.\n");
    }

    // 3. Cleanup
    supabase_free(client);
    return 0;
}
```

## 📂 Project Structure

Plaintext

```
supalib/
├── include/           # Header files (.h)
│   ├── cJSON.h        # Vendored JSON parser
│   ├── supabase.h     # Public API
│   └── private_key.h  # Configuration (Credentials)
├── src/               # Source files (.c)
│   ├── cJSON.c        # Vendored JSON implementation
│   └── supabase.c     # Supabase client implementation
├── main.c             # CLI Demo / Example usage
├── Makefile           # Cross-platform build configuration
└── README.md          # Documentation
```

## 🤝 Contributing

Contributions are welcome! Please open an issue or submit a pull request.

1. Fork the repo
    
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
    
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
    
4. Push to the branch (`git push origin feature/amazing-feature`)
    
5. Open a Pull Request
    

## 📄 License

Distributed under the MIT License. See `LICENSE` for more information.
