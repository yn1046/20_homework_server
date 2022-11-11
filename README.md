## Информация о проекте

Данный проект разрабатывался с использованием CMake под WSL2 (Ubuntu). Он использует пакетный
менеджер *<span style="color:#03dbfc">conan</span>*.

Поэтому для установки пакетов следует выполнить готовый скрипт `install_pkg.sh`:

```shell
$ ./install_pkg.sh
```

##### Список участников команды

Vasiliy Nesterovich

##### Имя тимлида (по желанию)

Vasiliy Nesterovich

##### Описание выбранной идеи решения

- Хранить даннные решено в JSON — для простоты и большей переносимости. С этой целью установлена
  библиотека *<span style="color:#03dbfc">nlohmann/json</span>*.
- Пароли, разумеется, лучше хешировать. Используется bcrypt из *<span style="color:#03dbfc">Botan</span>*.

##### Описание пользовательских типов и функций в проекте

**Модели:** _User_. Описывает пользователей.

**Сервисы:** ConnectionService, UserService. Первый отвечает за сетевую работу, второй — за хранение пользователей.

`Методы ConnectionService`

```c++
// Создать сокет и ожидать подключения (адрес и порт заданы в препроцессоре).
void connect();

// Принять входящее подключение.
int accept_client();

// Отправить клиенту сообщение произвольного типа.
template<typename T>
void send_message(int client_socket, T value);

// Отправить клиенту строку.
void send_message_string(int client_socket, const string &str);

// Получить от клиента сообщение произвольного типа.
template<typename T>
T receive_message(int client_socket);

// Получить от клиента строку.
string receive_message_string(int client_socket);

// Закрыть соединение с клиентом.
void close_client(int client_socket);

// Закрыть сокет.
void shutdown();
```

`Методы UserService`

```c++
// Аналогично предыдущему сервису.
void initialize();

// Создать нового пользователя (записать в файл).
User add_user(User &user);

// Получить объект пользователя по логину.
User get_user(string &login);

// Получить набор всех пользователей системы.
vector<User> get_users();

// Проверить, есть ли в системе пользователь с данным логином.
bool find_user(string &login);
```

**Контроллеры:** весь функционал данного чата управляется _ChatController_.

`Методы ChatController`

```c++
// Открыть сокет и начать принимать подключения.
void initialize();

// Начать обслуживать входящие подключения.
void accept_connections();

// Обработать подключившегося клиента.
void handle_client(int client_socket, uint32_t id);

// Выполнить регистрацию клиента.
void do_signup(uint32_t id);

// Выполнить вход клиента.
void do_login(uint32_t id);

// Отключить клиента.
void do_disconnect(uint32_t id);

// Ожидать очередное сообщение от клиента и затем обработать его.
void expect_message(uint32_t id);

// Обработать личное сообщение от одного клиента к другому.
void do_personal_message(uint32_t id, const string &message);

// Послать обработанное личное сообщение. 
void send_message(uint32_t id, uint32_t to_id, const string &message);

// Послать сообщение в общий чат.
void broadcast_message(uint32_t id, const string &message);

// Послать уведомление всем клиентам.
void broadcast_alert(const string &text);

// Сгенерировать хэш пароля.
static string gen_password(const string &password);

// Остановить сервер.
void do_stop();
```

##### Пояснение, как были распределены задачи в команде (кто какую часть проекта реализовывал)

Всё сделано в одиночку