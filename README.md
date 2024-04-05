# OS_HW_7
# Чечуров Владимир, ОС ДЗ-7

## Условие

Разработать программы клиента и сервера, взаимодействующих через разделяемую память с использованием функций POSIX. Клиент генерирует случайные числа в том же диапазоне, что и ранее рассмотренный пример. Сервер осуществляет их вывод. Предполагается (но специально не контролируется), что запускаются только один клиент и один сервер. Необходимо обеспечить корректное завершение работы для одного клиента и одного сервера, при котором удаляется сегмент разделяемой памяти. Предложить и реализовать свой вариант корректного завершения. Описать этот вариант в отчете.

## Решение

Данное задание выполнялось по критериям на оценку 10.

Реализовано клиент-серверное приложение, реализующее обмен данными через shared memory. Клиент (генерирует числа) и сервер (выводит числа) находятся в `client.c` и `server.c` соответственно.

Реализовано 2 способа завершения программы, файлы для этих реализаций находятся в `opt_1` и `opt_2`.

### opt_1

Пример работы (сначала запускается `client.c`):

![image-20240405205124906](/home/vlad/.config/Typora/typora-user-images/image-20240405205124906.png)

Здесь когда мы завершаем работу при нажатии клавиш `Ctrl+C` в консоли одного процесса, то мы меняем состояние флага в передаваемой структуре данных, что является сигналом для другого процесса, чтобы закрыть объект памяти и заверешить процесс, тем самым завершаются оба процесса и особождаются ресурсы.

### opt_2

Пример работы (сначала запускается `client.c`):

![image-20240405204539475](/home/vlad/.config/Typora/typora-user-images/image-20240405204539475.png)

Здесь когда мы завершаем работу при нажатии клавиш `Ctrl+C` в консоли одного процесса, то при помощи pipe (для этого мы указываем дополнительно ID процессов) в другом процессе идет закрытие объекта памяти и заверешение процесса, тем самым завершаются оба процесса и особождаются ресурсы.

### Общее

Для передачи данных используется структура:

```
typedef struct {
    int number;
    int flag; // в opt_2 отсутствует
} Package; // в opt_2 называется Data
```

