FROM gcc
COPY . /CLionProjects/progettoSO
WORKDIR /CLionProjects/progettoSO
RUN make all
CMD ["./out/master"]